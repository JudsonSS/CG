/**********************************************************************************
// Box (Arquivo de Cabeçalho)
//
// Criação:		27 Abr 2016
// Atualização:	25 Set 2021
// Compilador:	Visual C++ 2019
//
// Descrição:	Constrói um cubo 3D com vértices e índices
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

class Box : public App
{
private:
    ID3D12RootSignature* rootSignature = nullptr;
    ID3D12PipelineState* pipelineState = nullptr;
    Mesh* geometry = nullptr;

public:
    void Init();
    void Update();
    void Display();
    void Finalize();

    void BuildGeometry();
    void BuildRootSignature();
    void BuildPipelineState();    
};

// ------------------------------------------------------------------------------

#endif