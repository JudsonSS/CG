/**********************************************************************************
// Mesh (Código Fonte)
//
// Criação:		28 Abr 2016
// Atualização:	16 Mai 2016
// Compilador:	Visual C++ 14
//
// Descrição:	Representa uma malha 3D em Direct3D 12
//
**********************************************************************************/

#include "Mesh.h"

// -------------------------------------------------------------------------------

Mesh::Mesh(string Name)
{
	this->Name = Name;
	VertexBufferCPU = nullptr;
	IndexBufferCPU = nullptr;

	VertexBufferGPU = nullptr;
	IndexBufferGPU = nullptr;

	VertexBufferUploader = nullptr;
	IndexBufferUploader = nullptr;

	VertexByteStride = 0;
	VertexBufferByteSize = 0;
	IndexFormat = DXGI_FORMAT_R16_UINT;
	IndexBufferByteSize = 0;
}

// -------------------------------------------------------------------------------


Mesh::~Mesh()
{
	if (VertexBufferUploader)
		VertexBufferUploader->Release();
	if (IndexBufferUploader)
		IndexBufferUploader->Release();
	if (VertexBufferCPU)
		VertexBufferCPU->Release();
	if (IndexBufferCPU)
		IndexBufferCPU->Release();
	if (VertexBufferGPU)
		VertexBufferGPU->Release();
	if (IndexBufferGPU)
		IndexBufferGPU->Release();
}

// -------------------------------------------------------------------------------

D3D12_VERTEX_BUFFER_VIEW Mesh::VertexBufferView() const
{
	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
	vbv.StrideInBytes = VertexByteStride;
	vbv.SizeInBytes = VertexBufferByteSize;

	return vbv;
}

// -------------------------------------------------------------------------------

D3D12_INDEX_BUFFER_VIEW Mesh::IndexBufferView() const
{
	D3D12_INDEX_BUFFER_VIEW ibv;
	ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
	ibv.Format = IndexFormat;
	ibv.SizeInBytes = IndexBufferByteSize;

	return ibv;
}

// -------------------------------------------------------------------------------

