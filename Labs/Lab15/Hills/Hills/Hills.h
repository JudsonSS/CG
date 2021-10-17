/**********************************************************************************
// Hills (Arquivo de Cabe�alho)
//
// Cria��o:     27 Abr 2016
// Atualiza��o: 16 Out 2021
// Compilador:  Visual C++ 2019
//
// Descri��o:   Constr�i colinas a partir de um grid de v�rtices
//
**********************************************************************************/

#ifndef HILLS_H
#define HILLS_H

#include "DXUT.h"
#include <D3DCompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
using namespace DirectX;

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

class Hills : public App
{
private:
    ID3D12RootSignature* rootSignature = nullptr;
    ID3D12PipelineState* pipelineState = nullptr;

    ID3D12DescriptorHeap* cbufferHeap = nullptr;
    ID3D12Resource* cbufferUpload = nullptr;
    byte* cbufferData = nullptr;

    Mesh* geometry = nullptr;
    Timer timer;
    bool spinning = true;

    XMFLOAT4X4 World = {};
    XMFLOAT4X4 View = {};
    XMFLOAT4X4 Proj = {};

    float theta = 0;
    float phi = 0;
    float radius = 0;

    float lastMousePosX = 0;
    float lastMousePosY = 0;

public:
    void Init();
    void Update();
    void Draw();
    void Finalize();

    void BuildDescriptorHeaps();
    void BuildConstantBuffers();
    void BuildRootSignature();
    void BuildGeometry();
    void BuildPipelineState();    
};

// ------------------------------------------------------------------------------

#endif