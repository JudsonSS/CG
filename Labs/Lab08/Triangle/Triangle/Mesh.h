/**********************************************************************************
// Mesh (Arquivo de Cabe�alho)
//
// Cria��o:		28 Abr 2016
// Atualiza��o:	22 Jul 2020
// Compilador:	Visual C++ 19
//
// Descri��o:	Representa uma malha 3D em Direct3D 12
//
**********************************************************************************/

#ifndef DXUT_MESH_H_
#define DXUT_MESH_H_

// -------------------------------------------------------------------------------

#include <d3d12.h>
#include "Types.h"
#include <string>
#include <unordered_map>
using std::unordered_map;
using std::string;

// -------------------------------------------------------------------------------

struct SubMesh
{
	uint BaseVertexLocation = 0;
};

// -------------------------------------------------------------------------------

struct Mesh
{
	// identificador para recuperar a malha pelo seu nome
	string Id;

	// um Blob � usado para guardar dados de um tipo qualquer
	// o cliente deve fazer o cast apropriado a partir do Blob
	
	// buffers na CPU
	ID3DBlob* VertexBufferCPU;

	// buffers na GPU
	ID3D12Resource* VertexBufferGPU;

	// buffers de Upload CPU -> GPU
	ID3D12Resource* VertexBufferUploader;

	// caracter�sticas dos vertex buffers
	uint VertexByteStride;
	uint VertexBufferByteSize;

	// uma Mesh pode armazenar m�ltiplas SubMesh's em um buffer
	unordered_map<std::string, SubMesh> SubMesh;

	Mesh(string Name);
	~Mesh();

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;

};

// -------------------------------------------------------------------------------

#endif

