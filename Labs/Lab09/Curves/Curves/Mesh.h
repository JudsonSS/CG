/**********************************************************************************
// Mesh (Arquivo de Cabeçalho)
//
// Criação:		28 Abr 2016
// Atualização:	26 Jul 2020
// Compilador:	Visual C++ 19
//
// Descrição:	Representa uma malha 3D em Direct3D 12
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
	string Id;

	// um Blob é usado para guardar dados de um tipo qualquer
	// o cliente deve fazer o cast apropriado a partir do Blob
	
	// buffers na CPU
	ID3DBlob* VertexBufferCPU;

	// buffers de Upload CPU -> GPU
	ID3D12Resource* VertexBufferUpload;

	// buffers na GPU
	ID3D12Resource* VertexBufferGPU;

	// características dos vertex buffers
	uint VertexByteStride;
	uint VertexBufferSize;

	Mesh(string Name);
	~Mesh();

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;

};

// -------------------------------------------------------------------------------

#endif

