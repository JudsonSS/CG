/**********************************************************************************
// Geometry (Código Fonte)
//
// Criação:     03 Fev 2013
// Atualização: 11 Out 2021
// Compilador:  Visual C++ 2019
//
// Descrição:   Define vértices e índices para várias geometrias
//
**********************************************************************************/

#include "Geometry.h"

// ==============================================================================
// Geometry
// ==============================================================================

Geometry::Geometry() :
    position(0, 0, 0),
    type(UNKNOWN_T)
{
}

// ------------------------------------------------------------------------------

Geometry::~Geometry()
{
}

// ------------------------------------------------------------------------------

void Geometry::Subdivide()
{
    // salva uma cópia da geometria original
    vector <Vertex> verticesCopy = vertices;
    vector <uint> indicesCopy = indices;

    vertices.resize(0);
    indices.resize(0);

    //       v1
    //       *
    //      / \
    //     /   \
    //  m0*-----*m1
    //   / \   / \
    //  /   \ /   \
    // *-----*-----*
    // v0    m2     v2

    uint numTris = (uint)indicesCopy.size() / 3;
    for (uint i = 0; i < numTris; ++i)
    {
        Vertex v0 = verticesCopy[indicesCopy[size_t(i) * 3 + 0]];
        Vertex v1 = verticesCopy[indicesCopy[size_t(i) * 3 + 1]];
        Vertex v2 = verticesCopy[indicesCopy[size_t(i) * 3 + 2]];

        // acha os pontos centrais de cada aresta
        Vertex m0, m1, m2;
        XMStoreFloat3(&m0.pos, 0.5f * (XMLoadFloat3(&v0.pos) + XMLoadFloat3(&v1.pos)));
        XMStoreFloat3(&m1.pos, 0.5f * (XMLoadFloat3(&v1.pos) + XMLoadFloat3(&v2.pos)));
        XMStoreFloat3(&m2.pos, 0.5f * (XMLoadFloat3(&v0.pos) + XMLoadFloat3(&v2.pos)));

        // adiciona nova geometria
        vertices.push_back(v0); // 0
        vertices.push_back(v1); // 1
        vertices.push_back(v2); // 2
        vertices.push_back(m0); // 3
        vertices.push_back(m1); // 4
        vertices.push_back(m2); // 5

        indices.push_back(i * 6 + 0);
        indices.push_back(i * 6 + 3);
        indices.push_back(i * 6 + 5);

        indices.push_back(i * 6 + 3);
        indices.push_back(i * 6 + 4);
        indices.push_back(i * 6 + 5);

        indices.push_back(i * 6 + 5);
        indices.push_back(i * 6 + 4);
        indices.push_back(i * 6 + 2);

        indices.push_back(i * 6 + 3);
        indices.push_back(i * 6 + 1);
        indices.push_back(i * 6 + 4);
    }
}


// ==============================================================================
// Box
// ==============================================================================

Box::Box()
{
    type = BOX_T;
}

// ------------------------------------------------------------------------------

Box::Box(float width, float height, float depth)
{
    type = BOX_T;

    float w = 0.5f * width;
    float h = 0.5f * height;
    float d = 0.5f * depth;

    // cria os vértices da geometria
    Vertex boxVertices[8] =
    {
        { XMFLOAT3(-w, -h, -d), XMFLOAT4(Colors::Yellow) },
        { XMFLOAT3(-w, +h, -d), XMFLOAT4(Colors::Yellow) },
        { XMFLOAT3(+w, +h, -d), XMFLOAT4(Colors::Yellow) },
        { XMFLOAT3(+w, -h, -d), XMFLOAT4(Colors::Yellow) },
        { XMFLOAT3(-w, -h, +d), XMFLOAT4(Colors::Yellow) },
        { XMFLOAT3(-w, +h, +d), XMFLOAT4(Colors::Yellow) },
        { XMFLOAT3(+w, +h, +d), XMFLOAT4(Colors::Yellow) },
        { XMFLOAT3(+w, -h, +d), XMFLOAT4(Colors::Yellow) }
    };

    // insere vértices na malha
    for (const Vertex & v : boxVertices)
        vertices.push_back(v);

    // índices indicam como os vértices são interligados
    uint boxIndices[36] =
    {
        // front face
        0, 1, 2,
        0, 2, 3,

        // back face
        4, 7, 5,
        7, 6, 5,

        // left face
        4, 5, 1,
        4, 1, 0,

        // right face
        3, 2, 6,
        3, 6, 7,

        // top face
        1, 5, 6,
        1, 6, 2,

        // bottom face
        4, 0, 3,
        4, 3, 7
    };

    // insere índices na malha
    for (ushort i : boxIndices)
        indices.push_back(i);
}

// ==============================================================================
// Cylinder
// ==============================================================================

Cylinder::Cylinder()
{
    type = CYLINDER_T;
}

// ------------------------------------------------------------------------------

Cylinder::Cylinder(float bottom, float top, float height, uint sliceCount, uint stackCount)
{
    type = CYLINDER_T;

    // altura de uma camada
    float stackHeight = height / stackCount;

    // incremento no raio de cada camada
    float radiusStep = (top - bottom) / stackCount;

    // número de anéis do cilindro
    uint ringCount = stackCount + 1;

    // calcula vértices de cada anel
    for (uint i = 0; i < ringCount; ++i)
    {
        float y = -0.5f * height + i * stackHeight;
        float r = bottom + i * radiusStep;
        float theta = 2.0f * XM_PI / sliceCount;

        for (uint j = 0; j <= sliceCount; ++j)
        {
            float c = cosf(j * theta);
            float s = sinf(j * theta);

            Vertex vertex;
            vertex.pos = XMFLOAT3(r * c, y, r * s);
            vertex.color = XMFLOAT4(Colors::Yellow);
            vertices.push_back(vertex);
        }
    }

    // número de vértices em cada anel do cilindro
    uint ringVertexCount = sliceCount + 1;

    // calcula índices para cada camada
    for (uint i = 0; i < stackCount; ++i)
    {
        for (uint j = 0; j < sliceCount; ++j)
        {
            indices.push_back(i * ringVertexCount + j);
            indices.push_back((i + 1) * ringVertexCount + j);
            indices.push_back((i + 1) * ringVertexCount + j + 1);
            indices.push_back(i * ringVertexCount + j);
            indices.push_back((i + 1) * ringVertexCount + j + 1);
            indices.push_back(i * ringVertexCount + j + 1);
        }
    }

    // constrói vértices das tampas do cilindro
    for (uint k = 0; k < 2; ++k)
    {
        uint baseIndex = uint(vertices.size());

        float y = (k - 0.5f) * height;
        float theta = 2.0f * XM_PI / sliceCount;
        float r = (k ? top : bottom);
        Vertex vertex;

        for (uint i = 0; i <= sliceCount; i++)
        {
            float x = r * cosf(i * theta);
            float z = r * sinf(i * theta);

            vertex.pos = XMFLOAT3(x, y, z);
            vertex.color = XMFLOAT4(Colors::Yellow);
            vertices.push_back(vertex);
        }

        // vértice central da tampa
        vertex.pos = XMFLOAT3(0.0f, y, 0.0f);
        vertex.color = XMFLOAT4(Colors::Yellow);
        vertices.push_back(vertex);

        uint centerIndex = uint(vertices.size() - 1);

        // indices para a tampa
        for (uint i = 0; i < sliceCount; ++i)
        {
            indices.push_back(centerIndex);
            indices.push_back(baseIndex + i + k);
            indices.push_back(baseIndex + i + 1 - k);
        }
    }
}

// ==============================================================================
// Sphere
// ==============================================================================

Sphere::Sphere()
{
    type = SPHERE_T;
}

// ------------------------------------------------------------------------------

Sphere::Sphere(float radius, uint sliceCount, uint stackCount)
{
    type = SPHERE_T;

    // calcula os vértice iniciando no pólo superior e descendo pelas camadas

    Vertex topVertex;
    topVertex.pos = XMFLOAT3(0.0f, radius, 0.0f);
    topVertex.color = XMFLOAT4(Colors::Yellow);

    Vertex bottomVertex;
    bottomVertex.pos = XMFLOAT3(0.0f, -radius, 0.0f);
    bottomVertex.color = XMFLOAT4(Colors::Yellow);

    vertices.push_back(topVertex);

    float phiStep = XM_PI / stackCount;
    float thetaStep = 2.0f * XM_PI / sliceCount;

    // calcula os vértices para cada anel (não conta os pólos como anéis)
    for (uint i = 1; i <= stackCount - 1; ++i)
    {
        float phi = i * phiStep;

        // vértices do anel
        for (uint j = 0; j <= sliceCount; ++j)
        {
            float theta = j * thetaStep;

            Vertex v;

            // coordenadas esféricas para cartesianas
            v.pos.x = radius * sinf(phi) * cosf(theta);
            v.pos.y = radius * cosf(phi);
            v.pos.z = radius * sinf(phi) * sinf(theta);

            v.color = XMFLOAT4(Colors::Yellow);

            vertices.push_back(v);
        }
    }

    vertices.push_back(bottomVertex);

    // calcula os índices da camada superior 
    // esta camada conecta o pólo superior ao primeiro anel
    for (uint i = 1; i <= sliceCount; ++i)
    {
        indices.push_back(0);
        indices.push_back(i + 1);
        indices.push_back(i);
    }

    // calcula os índices para as camadas internas (não conectadas aos pólos)
    uint baseIndex = 1;
    uint ringVertexCount = sliceCount + 1;
    for (uint i = 0; i < stackCount - 2; ++i)
    {
        for (uint j = 0; j < sliceCount; ++j)
        {
            indices.push_back(baseIndex + i * ringVertexCount + j);
            indices.push_back(baseIndex + i * ringVertexCount + j + 1);
            indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

            indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
            indices.push_back(baseIndex + i * ringVertexCount + j + 1);
            indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
        }
    }

    // calcula os índices da camada inferior 
    // esta camada conecta o pólo inferior ao último anel

    // pólo inferior é adicionado por último
    uint southPoleIndex = (uint)vertices.size() - 1;

    // se posiciona nos índices do primeiro vértice do último anel
    baseIndex = southPoleIndex - ringVertexCount;

    for (uint i = 0; i < sliceCount; ++i)
    {
        indices.push_back(southPoleIndex);
        indices.push_back(baseIndex + i);
        indices.push_back(baseIndex + i + 1);
    }
}

// ==============================================================================
// GeoSphere
// ==============================================================================

GeoSphere::GeoSphere()
{
    type = GEOSPHERE_T;
}

// ------------------------------------------------------------------------------

GeoSphere::GeoSphere(float radius, uint subdivisions)
{
    type = GEOSPHERE_T;

    // limita o número de subdivisões
    subdivisions = (subdivisions > 6U ? 6U : subdivisions);

    // aproxima uma esfera pela subdivisão de um icosaédro
    const float X = 0.525731f;
    const float Z = 0.850651f;

    // vértices do icosaédro
    XMFLOAT3 pos[12] =
    {
        XMFLOAT3(-X, 0.0f, Z),  XMFLOAT3(X, 0.0f, Z),
        XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
        XMFLOAT3(0.0f, Z, X),   XMFLOAT3(0.0f, Z, -X),
        XMFLOAT3(0.0f, -Z, X),  XMFLOAT3(0.0f, -Z, -X),
        XMFLOAT3(Z, X, 0.0f),   XMFLOAT3(-Z, X, 0.0f),
        XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)
    };

    // índices do icosaédro
    uint k[60] =
    {
        1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
        1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
        3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
        10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
    };

    // ajusta e inicializa vetores de vértices e índices
    vertices.resize(12);
    indices.assign(&k[0], &k[60]);

    for (uint i = 0; i < 12; ++i)
        vertices[i].pos = pos[i];

    // subdivide cada triângulo do icosaedro um certo número de vezes
    for (uint i = 0; i < subdivisions; ++i)
        Subdivide();

    // projeta os vértices em uma esfera e ajusta a escala
    for (uint i = 0; i < vertices.size(); ++i)
    {
        // normaliza vetor (ponto)
        XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&vertices[i].pos));

        // projeta na esfera
        XMVECTOR p = radius * n;

        XMStoreFloat3(&vertices[i].pos, p);
        vertices[i].color = XMFLOAT4(Colors::Yellow);
    }
}


// ==============================================================================
// Grid
// ==============================================================================

Grid::Grid()
{
    type = GRID_T;
}

// ------------------------------------------------------------------------------

Grid::Grid(float width, float depth, uint m, uint n)
{
    type = GRID_T;

    uint vertexCount = m * n;
    uint triangleCount = 2 * (m - 1) * (n - 1);

    // cria os vértices
    float halfWidth = 0.5f * width;
    float halfDepth = 0.5f * depth;

    float dx = width / (n - 1);
    float dz = depth / (m - 1);

    // ajusta tamanho do vetor de vértices
    vertices.resize(vertexCount);

    for (uint i = 0; i < m; ++i)
    {
        float z = halfDepth - i * dz;

        for (uint j = 0; j < n; ++j)
        {
            float x = -halfWidth + j * dx;

            // define vértices do grid
            vertices[size_t(i) * n + j].pos = XMFLOAT3(x, 0.0f, z);
            vertices[size_t(i) * n + j].color = XMFLOAT4(Colors::Yellow);
        }
    }

    // ajusta tamanho do vetor de índices
    indices.resize(size_t(triangleCount) * 3);

    size_t k = 0;

    for (uint i = 0; i < m - 1; ++i)
    {
        for (uint j = 0; j < n - 1; ++j)
        {
            indices[k] = i * n + j;
            indices[k + 1] = i * n + j + 1;
            indices[k + 2] = (i + 1) * n + j;
            indices[k + 3] = (i + 1) * n + j;
            indices[k + 4] = i * n + j + 1;
            indices[k + 5] = (i + 1) * n + j + 1;

            k += 6; // próximo quad
        }
    }
}

// ==============================================================================
// Quad
// ==============================================================================

Quad::Quad()
{
    type = QUAD_T;
}

// ------------------------------------------------------------------------------

Quad::Quad(float width, float height)
{
    type = QUAD_T;

    float w = 0.5f * width;
    float h = 0.5f * height;

    // cria vertex buffer
    Vertex quadVertices[4] =
    {
        { XMFLOAT3(-w, -h, 0.0f), XMFLOAT4(Colors::Yellow) },
        { XMFLOAT3(-w, +h, 0.0f), XMFLOAT4(Colors::Yellow) },
        { XMFLOAT3(+w, +h, 0.0f), XMFLOAT4(Colors::Yellow) },
        { XMFLOAT3(+w, -h, 0.0f), XMFLOAT4(Colors::Yellow) }
    };

    // insere vértices na malha
    for (const Vertex & v : quadVertices)
        vertices.push_back(v);

    // cria o index buffer
    uint quadIndices[6] =
    {
        0, 1, 2,
        0, 2, 3
    };

    // insere índices na malha
    for (uint i : quadIndices)
        indices.push_back(i);
}

// -------------------------------------------------------------------------------