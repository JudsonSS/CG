/**********************************************************************************
// Game (C�digo Fonte)
//
// Cria��o:		08 Dez 2012
// Atualiza��o:	27 Abr 2016
// Compilador:	Visual C++ 14
//
// Descri��o:	Uma classe abstrata para representar um jogo.
//
//              Para criar um jogo o programador deve criar uma classe derivada
//              de Game e sobrescrever os m�todos Init, Update, Draw e Finalize.
//              Para rodar o jogo, deve-se passar o objeto Game para o m�todo
//              Start() de um objeto Engine.
//
**********************************************************************************/

#include "Game.h"
#include "Engine.h"

// -------------------------------------------------------------------------------
// Inicializa��o de membros est�ticos da classe

Window   * Game::window   = nullptr;			// janela do jogo
Input    * Game::input    = nullptr;			// dispositivo de entrada
Graphics * Game::graphics = nullptr;			// dispositivo gr�fico
float    & Game::gameTime = Engine::gameTime;	// tempo do �ltimo frame

// -------------------------------------------------------------------------------

Game::Game()
{
	// assume que a inst�ncia da engine �  
	// criada antes da inst�ncia do jogo

	if (!window)
		window = Engine::window;
	
	if (!input)
		input = Engine::input;

	if (!graphics)
		graphics = Engine::graphics;
}

// -------------------------------------------------------------------------------

Game::~Game()
{
}

// -------------------------------------------------------------------------------