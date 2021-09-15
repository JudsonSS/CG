/**********************************************************************************
// Mesh (Arquivo de Cabeçalho)
//
// Criação:		28 Abr 2016
// Atualização:	16 Mai 2016
// Compilador:	Visual C++ 14
//
// Descrição:	Representa uma malha 3D em Direct3D 12
//
**********************************************************************************/

#ifndef _DX12_MESH_H_
#define _DX12_MESH_H_

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
	uint IndexCount = 0;
	uint StartIndexLocation = 0;
	uint BaseVertexLocation = 0;
};

// -------------------------------------------------------------------------------

struct Mesh
{
	// para recuperar a malha pelo seu nome
	string Name;

	// o formato do buffer para vértices/índices pode ser genérico
	// e o cliente deve fazer o cast apropriado a partir do Blob  
	ID3DBlob * VertexBufferCPU;
	ID3DBlob * IndexBufferCPU;

	ID3D12Resource * VertexBufferGPU;
	ID3D12Resource * IndexBufferGPU;

	ID3D12Resource * VertexBufferUploader;
	ID3D12Resource * IndexBufferUploader;

	// dados sobre os buffers
	uint VertexByteStride;
	uint VertexBufferByteSize;
	DXGI_FORMAT IndexFormat;
	uint IndexBufferByteSize;

	// uma Mesh poder armazenar múltiplas SubMesh's em um buffer
	unordered_map<std::string, SubMesh> SubMesh;

	Mesh(string Name);
	~Mesh(); 

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView() const;
};

// -------------------------------------------------------------------------------

#endif
