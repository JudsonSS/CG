/**********************************************************************************
// Box (Arquivo de Cabeçalho)
//
// Criação:		27 Abr 2016
// Atualização:	27 Abr 2016
// Compilador:	Visual C++ 14
//
// Descrição:	Constrói um cubo 3D usando o Direct3D 12
//
**********************************************************************************/

#ifndef _DX12_BOX_H_
#define _DX12_BOX_H_

#include "Engine.h"
#include "Game.h"
#include "Mesh.h"
#include "Resources.h"
#include <D3DCompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXPackedVector.h>
using namespace DirectX;
using namespace DirectX::PackedVector;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct ObjectConstants
{
	XMFLOAT4X4 WorldViewProj =
	{ 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };
};

// ------------------------------------------------------------------------------

class Box : public Game
{
private:
	ID3D12RootSignature      * rootSignature;
	ID3D12DescriptorHeap     * constantBufferHeap;
	ID3D12Resource           * constantBufferUpload;
	ID3DBlob                 * vertexShaderByteCode;
	ID3DBlob                 * pixelShaderByteCode;
	ID3D12PipelineState      * PSO;
	D3D12_INPUT_ELEMENT_DESC   inputLayout[2];

	Mesh * boxGeo;
	BYTE * constantBufferData;

	XMFLOAT4X4 World;
	XMFLOAT4X4 View;
	XMFLOAT4X4 Proj;

	float theta;
	float phi;
	float radius;

	float lastMousePosX;
	float lastMousePosY;

public:
	void Init();
	void Update();
	void Draw();
	void Finalize();

	void BuildDescriptorHeaps();
	void BuildConstantBuffers();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildBoxGeometry();
	void BuildPSO();
};

// ------------------------------------------------------------------------------

#endif