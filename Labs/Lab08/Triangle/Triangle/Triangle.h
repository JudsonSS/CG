/**********************************************************************************
// Triangle (Arquivo de Cabe�alho)
//
// Cria��o:		22 Jul 2020
// Atualiza��o:	22 Jul 2020
// Compilador:	Visual C++ 2019
//
// Descri��o:	Constr�i um Tri�ngulo usando o Direct3D 12
//
**********************************************************************************/

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "DXUT.h"
#include <D3DCompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
using namespace DirectX;

// ------------------------------------------------------------------------------

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

// ------------------------------------------------------------------------------

class Triangle : public App
{
private:
	ID3D12RootSignature* rootSignature;
	D3D12_INPUT_ELEMENT_DESC inputLayout[2];
	ID3DBlob* vertexShaderByteCode;
	ID3DBlob* pixelShaderByteCode;
	ID3D12PipelineState* PSO;
	Mesh* geometry;

public:
	void Init();
	void Update();
	void Draw();
	void Finalize();

	void BuildRootSignature();
	void BuildInputLayout();
	void BuildShaders();
	void BuildGeometry();
	void BuildPSO();
};

// ------------------------------------------------------------------------------

#endif