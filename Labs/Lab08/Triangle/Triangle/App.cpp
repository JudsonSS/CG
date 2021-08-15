/**********************************************************************************
// App (C�digo Fonte)
//
// Cria��o:     11 Jan 2020
// Atualiza��o: 15 Ago 2021
// Compilador:  Visual C++ 2019
//
// Descri��o:   Uma classe abstrata para representar uma aplica��o
//
//              Todas as aplica��es derivadas da classe App devem
//              sobrescrever os m�todos Init, Update, Draw e Finalize.
//
**********************************************************************************/

#include "App.h"
#include "Engine.h"

// -------------------------------------------------------------------------------
// Inicializa��o de membros est�ticos da classe

Graphics* & App::graphics  = Engine::graphics;       // componente gr�fico 
Window*   & App::window    = Engine::window;         // janela da aplica��o
Input*    & App::input     = Engine::input;          // dispositivos de entrada
double    & App::frameTime = Engine::frameTime;      // tempo do �ltimo quadro

// -------------------------------------------------------------------------------

App::App()
{
}

// -------------------------------------------------------------------------------

App::~App()
{
}

// -------------------------------------------------------------------------------