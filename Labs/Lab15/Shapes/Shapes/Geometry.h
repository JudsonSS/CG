/**********************************************************************************
// Geometry (Arquivo de Cabe�alho)
//
// Cria��o:     03 Fev 2013
// Atualiza��o: 11 Out 2021
// Compilador:  Visual C++ 2019
//
// Descri��o:   Define v�rtices e �ndices para v�rias geometrias
//
**********************************************************************************/

#ifndef DXUT_GEOMETRY_H_
#define DXUT_GEOMETRY_H_

// -------------------------------------------------------------------------------

#include "Types.h"
#include <vector>
#include <DirectXMath.h>
#include <DirectXColors.h>
using namespace DirectX;
using std::vector;

// -------------------------------------------------------------------------------

struct Vertex
{
    XMFLOAT3 pos;
    XMFLOAT4 color;
};

// -------------------------------------------------------------------------------
// Geometry
// -------------------------------------------------------------------------------

class Geometry
{
protected:
    XMFLOAT3       position;                            // posi��o da geometria
    uint           type;                                // tipo da geometria
    vector<Vertex> vertices;                            // v�rtices da geometria
    vector<uint>   indices;                             // �ndices da geometria

    void Subdivide();                                   // subdivide os tri�ngulos

public:
    Geometry();                                         // construtor
    virtual ~Geometry();                                // destrutor

    virtual float X() const { return position.x; }      // coordenada x da geometria
    virtual float Y() const { return position.y; }      // coordenada y da geometria
    virtual float Z() const { return position.z; }      // coordenada z da geometria

    // retorna tipo da geometria
    virtual uint Type() const
    { return type; }

    // retorna posi��o da geometria
    virtual XMFLOAT3 Position() const
    { return position; }

    // move a geometria pelo delta (dx,dy,dz)
    virtual void Translate(float dx, float dy, float dz)
    { position.x += dx; position.y += dy; position.z += dz; }

    // move a geometria para a posi��o (px,py,pz)
    virtual void MoveTo(float px, float py, float pz)
    { position.x = px; position.y = py; position.z = pz; }

    // retorna v�rtices da geometria
    const Vertex* VertexData() const
    { return vertices.data(); }

    // retorna �ndices da geometria
    const uint* IndexData() const
    { return indices.data(); }

    // retorna n�mero de v�rtices
    uint VertexCount() const
    { return uint(vertices.size()); }

    // retorna n�mero de �ndices
    uint IndexCount() const
    { return uint(indices.size()); }
};

// -------------------------------------------------------------------------------
// Box
// -------------------------------------------------------------------------------

class Box : public Geometry
{
public:
    Box();
    Box(float width, float height, float depth);
};

// -------------------------------------------------------------------------------
// Cylinder
// -------------------------------------------------------------------------------

class Cylinder : public Geometry
{
public:
    Cylinder();
    Cylinder(float bottom, float top, float height, uint sliceCount, uint stackCount);
};

// -------------------------------------------------------------------------------
// Sphere
// -------------------------------------------------------------------------------

class Sphere : public Geometry
{
public:
    Sphere();
    Sphere(float radius, uint sliceCount, uint stackCount);
};

// -------------------------------------------------------------------------------
// GeoSphere
// -------------------------------------------------------------------------------

class GeoSphere : public Geometry
{
public:
    GeoSphere();
    GeoSphere(float radius, uint subdivisions);
};


// -------------------------------------------------------------------------------
// Grid
// -------------------------------------------------------------------------------

class Grid : public Geometry
{
public:
    Grid();
    Grid(float width, float depth, uint m, uint n);
};

// -------------------------------------------------------------------------------
// Quad
// -------------------------------------------------------------------------------

class Quad : public Geometry
{
public:
    Quad();
    Quad(float width, float height);
};

// -------------------------------------------------------------------------------

#endif
