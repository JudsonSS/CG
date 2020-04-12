/**********************************************************************************
// Engine (Arquivo de Cabeçalho)
//
// Criação:		11 Jan 2020
// Atualização:	15 Jan 2020
// Compilador:	Visual C++ 2019
//
// Descrição:	A Engine roda aplicações criadas a partir da classe App. 
//				
//              Para usar a Engine crie uma instância e chame o método
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
	static App * app;					// aplicação a ser executada
	int Loop();							// inicia laço principal do motor

public:
	static Window* window;				// janela da aplicação
	static Input* input;				// entrada da aplicação

	Engine();							// construtor
	~Engine();							// destrutor

	int Start(App * application);		// inicia o execução da aplicação

	// trata eventos do Windows
	static LRESULT CALLBACK EngineProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

// ---------------------------------------------------------------------------------

#endif