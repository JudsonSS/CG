/**********************************************************************************
// Mesh (Arquivo de Cabeçalho)
//
// Criação:     28 Abr 2016
// Atualização: 15 Ago 2021
// Compilador:  Visual C++ 19
//
// Descrição:   Representa uma malha 3D em Direct3D 12
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

    // um Blob é usado para guardar dados de um tipo qualquer
    // o cliente deve fazer o cast apropriado a partir do Blob
    
    // buffers na CPU
    ID3DBlob* vertexBufferCPU;

    // buffers de Upload CPU -> GPU
    ID3D12Resource* vertexBufferUpload;

    // buffers na GPU
    ID3D12Resource* vertexBufferGPU;

    // descritor do vertex buffer
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

    // características dos vertex buffers
    uint vertexByteStride;
    uint vertexBufferSize;

    // construtor e destrutor
    Mesh(string name);
    ~Mesh();

    // retorna descritor (view) do Vertex Buffer
    D3D12_VERTEX_BUFFER_VIEW * VertexBufferView();
};

// -------------------------------------------------------------------------------

#endif

