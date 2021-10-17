/**********************************************************************************
// Mesh (Código Fonte)
//
// Criação:     28 Abr 2016
// Atualização: 11 Out 2021
// Compilador:  Visual C++ 2019
//
// Descrição:   Representa uma malha 3D
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

    indexBufferCPU = nullptr;
    indexBufferGPU = nullptr;
    indexBufferUpload = nullptr;

    ZeroMemory(&vertexBufferView, sizeof(D3D12_VERTEX_BUFFER_VIEW));
    ZeroMemory(&indexBufferView, sizeof(D3D12_INDEX_BUFFER_VIEW));
    ZeroMemory(&indexFormat, sizeof(DXGI_FORMAT));
    vertexByteStride = 0;
    vertexBufferSize = 0;    
    indexBufferSize = 0;
}

// -------------------------------------------------------------------------------

Mesh::~Mesh()
{
    if (vertexBufferUpload) vertexBufferUpload->Release();
    if (vertexBufferGPU) vertexBufferGPU->Release();
    if (vertexBufferCPU) vertexBufferCPU->Release();

    if (indexBufferUpload) indexBufferUpload->Release();
    if (indexBufferGPU) indexBufferGPU->Release();
    if (indexBufferCPU) indexBufferCPU->Release();
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

D3D12_INDEX_BUFFER_VIEW * Mesh::IndexBufferView()
{
    indexBufferView.BufferLocation = indexBufferGPU->GetGPUVirtualAddress();
    indexBufferView.Format = indexFormat;
    indexBufferView.SizeInBytes = indexBufferSize;

    return &indexBufferView;
}

// -------------------------------------------------------------------------------

