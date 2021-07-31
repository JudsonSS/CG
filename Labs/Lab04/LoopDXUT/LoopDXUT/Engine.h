/**********************************************************************************
// Engine (Arquivo de Cabe�alho)
//
// Cria��o:		11 Jan 2020
// Atualiza��o:	31 Jul 2021
// Compilador:	Visual C++ 2019
//
// Descri��o:	A Engine roda aplica��es criadas a partir da classe App. 
//				
//              Para usar a Engine crie uma inst�ncia e chame o m�todo
//				Start() com um objeto derivado da classe App.
//
**********************************************************************************/

#ifndef DXUT_ENGINE_H
#define DXUT_ENGINE_H

// ---------------------------------------------------------------------------------

#include "App.h"
#include "Window.h"

// ---------------------------------------------------------------------------------

class Engine
{
private:
	static App * app;					// aplica��o a ser executada
	int Loop();							// la�o principal do motor

public:
	static Window* window;				// janela da aplica��o
	static Input* input;				// dispositivos de entrada da aplica��o

	Engine();							// construtor
	~Engine();							// destrutor

	int Start(App * application);		// inicia o execu��o da aplica��o

	// trata eventos do Windows
	static LRESULT CALLBACK EngineProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

// ---------------------------------------------------------------------------------

#endif