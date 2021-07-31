/**********************************************************************************
// App (Código Fonte)
//
// Criação:		11 Jan 2020
// Atualização:	31 Jul 2021
// Compilador:	Visual C++ 2019
//
// Descrição:	Uma classe abstrata para representar uma aplicação
//
//              Todas as aplicações derivadas da classe App devem
//              sobrescrever os métodos Init, Update, Draw e Finalize.
//
**********************************************************************************/

#include "App.h"
#include "Engine.h"

// -------------------------------------------------------------------------------
// Inicialização de membros estáticos da classe

Window* App::window = nullptr;				// ponteiro para a janela
Input* App::input = nullptr;				// ponteiro para a entrada
float& App::frameTime = Engine::frameTime;	// tempo do último quadro

// -------------------------------------------------------------------------------

App::App()
{
	if (!window)
	{
		// ATENÇÃO: assume que a instância da engine é  
		// criada antes da instância da aplicação
		window = Engine::window;
		input = Engine::input;
	}
}

// -------------------------------------------------------------------------------

App::~App()
{
}

// -------------------------------------------------------------------------------