/**********************************************************************************
// Box (Arquivo de Cabe�alho)
//
// Cria��o:		13 Fev 2020
// Atualiza��o:	13 Fev 2020
// Compilador:	Visual C++ 2019
//
// Descri��o:	Constr�i um cubo 3D usando o Direct3D 12
//
**********************************************************************************/

#ifndef BOX_H
#define BOX_H

#include "DXUT.h"
#include <DirectXMath.h>
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
	D3D12_INPUT_ELEMENT_DESC inputLayout[2];

public:
	void Init();
	void Update();
	void Draw();
	void Finalize();

	void BuildInputLayout();
};

// ------------------------------------------------------------------------------

#endif