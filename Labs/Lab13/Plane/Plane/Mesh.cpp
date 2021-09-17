/**********************************************************************************
// Mesh (Código Fonte)
//
// Criação:     28 Abr 2016
// Atualização: 15 Ago 2021
// Compilador:  Visual C++ 19
//
// Descrição:   Representa uma malha 3D em Direct3D 12
//
**********************************************************************************/

#include "Mesh.h"

// -------------------------------------------------------------------------------

Mesh::Mesh(string name)
{
    id = name;

    vertexBufferCPU = nullptr;
    vertexBufferGPU = nullptr;
    vertexBufferUpload = nullptr;

    vertexByteStride = 0;
    vertexBufferSize = 0;
}

// -------------------------------------------------------------------------------

Mesh::~Mesh()
{
    if (vertexBufferUpload) vertexBufferUpload->Release();
    if (vertexBufferGPU) vertexBufferGPU->Release();
    if (vertexBufferCPU) vertexBufferCPU->Release();
}

// -------------------------------------------------------------------------------

D3D12_VERTEX_BUFFER_VIEW * Mesh::VertexBufferView()
{
    vertexBufferView.BufferLocation = vertexBufferGPU->GetGPUVirtualAddress();
    vertexBufferView.StrideInBytes = vertexByteStride;
    vertexBufferView.SizeInBytes = vertexBufferSize;

    return &vertexBufferView;
}

// -------------------------------------------------------------------------------

