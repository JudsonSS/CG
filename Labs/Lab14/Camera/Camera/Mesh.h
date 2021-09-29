/**********************************************************************************
// Mesh (Arquivo de Cabeçalho)
//
// Criação:     28 Abr 2016
// Atualização: 25 Set 2021
// Compilador:  Visual C++ 2019
//
// Descrição:   Representa uma malha 3D
//
**********************************************************************************/

#ifndef DXUT_MESH_H_
#define DXUT_MESH_H_

// -------------------------------------------------------------------------------

#include <d3d12.h>
#include "Types.h"
#include <string>
using std::string;

// -------------------------------------------------------------------------------

struct Mesh
{
    // identificador para recuperar a malha pelo seu nome
    string id;
    
    // buffers na CPU
    ID3DBlob* vertexBufferCPU;
    ID3DBlob* indexBufferCPU;

    // buffers de Upload CPU -> GPU
    ID3D12Resource* vertexBufferUpload;
    ID3D12Resource* indexBufferUpload;

    // buffers na GPU
    ID3D12Resource* vertexBufferGPU;
    ID3D12Resource* indexBufferGPU;

    // descritor do vertex buffer
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
    D3D12_INDEX_BUFFER_VIEW indexBufferView;

    // características do vertex buffer
    uint vertexByteStride;
    uint vertexBufferSize;

    // características do index buffer
    DXGI_FORMAT indexFormat;
    uint indexBufferSize;

    // construtor e destrutor
    Mesh(string name);
    ~Mesh();

    // retorna descritor (view) do Vertex Buffer
    D3D12_VERTEX_BUFFER_VIEW * VertexBufferView();
    D3D12_INDEX_BUFFER_VIEW * IndexBufferView();
};

// -------------------------------------------------------------------------------

#endif

