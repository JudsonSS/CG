/**********************************************************************************
// App (C�digo Fonte)
//
// Cria��o:		11 Jan 2020
// Atualiza��o:	31 Jul 2021
// Compilador:	Visual C++ 2019
//
// Descri��o:	Uma classe abstrata para representar uma aplica��o
//
//              Todas as aplica��es derivadas da classe App devem
//              sobrescrever os m�todos Init, Update, Draw e Finalize.
//
**********************************************************************************/

#include "App.h"
#include "Engine.h"

// -------------------------------------------------------------------------------
// Inicializa��o de membros est�ticos da classe

Window* App::window = nullptr;				// ponteiro para a janela
Input* App::input = nullptr;				// ponteiro para a entrada
float& App::frameTime = Engine::frameTime;	// tempo do �ltimo quadro

// -------------------------------------------------------------------------------

App::App()
{
	if (!window)
	{
		// ATEN��O: assume que a inst�ncia da engine �  
		// criada antes da inst�ncia da aplica��o
		window = Engine::window;
		input = Engine::input;
	}
}

// -------------------------------------------------------------------------------

App::~App()
{
}

// -------------------------------------------------------------------------------