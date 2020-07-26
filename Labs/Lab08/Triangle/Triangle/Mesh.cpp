/**********************************************************************************
// Mesh (Código Fonte)
//
// Criação:		28 Abr 2016
// Atualização:	26 Jul 2020
// Compilador:	Visual C++ 19
//
// Descrição:	Representa uma malha 3D em Direct3D 12
//
**********************************************************************************/

#include "Mesh.h"

// -------------------------------------------------------------------------------

Mesh::Mesh(string Name)
{
	Id = Name;

	VertexBufferCPU = nullptr;
	VertexBufferGPU = nullptr;
	VertexBufferUpload = nullptr;

	VertexByteStride = 0;
	VertexBufferSize = 0;
}

// -------------------------------------------------------------------------------

Mesh::~Mesh()
{
	if (VertexBufferUpload) VertexBufferUpload->Release();
	if (VertexBufferGPU) VertexBufferGPU->Release();
	if (VertexBufferCPU) VertexBufferCPU->Release();
}

// -------------------------------------------------------------------------------

D3D12_VERTEX_BUFFER_VIEW Mesh::VertexBufferView() const
{
	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
	vbv.StrideInBytes = VertexByteStride;
	vbv.SizeInBytes = VertexBufferSize;

	return vbv;
}

// -------------------------------------------------------------------------------

