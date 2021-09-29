/**********************************************************************************
// Box (Arquivo de Cabeçalho)
//
// Criação:		27 Abr 2016
// Atualização:	28 Set 2021
// Compilador:	Visual C++ 2019
//
// Descrição:	Desenha um cubo 3D usando o Vertex Shader para 
//              realizar a multiplicação pela matriz de projeção.
//
**********************************************************************************/

#ifndef BOX_H
#define BOX_H

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

struct ObjectConstants
{
    XMFLOAT4X4 WorldViewProj =
    { 1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f };
};

// ------------------------------------------------------------------------------

class Box : public App
{
private:
    ID3D12RootSignature* rootSignature = nullptr;
    ID3D12PipelineState* pipelineState = nullptr;
    Mesh* geometry = nullptr;

    ID3D12DescriptorHeap* constantBufferHeap = nullptr;
    ID3D12Resource* constantBufferUpload = nullptr;
    BYTE* constantBufferData = nullptr;

public:
    void Init();
    void Update();
    void Draw();
    void Finalize();

    void BuildConstantBuffers();
    void BuildGeometry();
    void BuildRootSignature();
    void BuildPipelineState();    
};

// ------------------------------------------------------------------------------

#endif