/**********************************************************************************
// Game (Código Fonte)
//
// Criação:		08 Dez 2012
// Atualização:	27 Abr 2016
// Compilador:	Visual C++ 14
//
// Descrição:	Uma classe abstrata para representar um jogo.
//
//              Para criar um jogo o programador deve criar uma classe derivada
//              de Game e sobrescrever os métodos Init, Update, Draw e Finalize.
//              Para rodar o jogo, deve-se passar o objeto Game para o método
//              Start() de um objeto Engine.
//
**********************************************************************************/

#include "Game.h"
#include "Engine.h"

// -------------------------------------------------------------------------------
// Inicialização de membros estáticos da classe

Window   * Game::window   = nullptr;			// janela do jogo
Input    * Game::input    = nullptr;			// dispositivo de entrada
Graphics * Game::graphics = nullptr;			// dispositivo gráfico
float    & Game::gameTime = Engine::gameTime;	// tempo do último frame

// -------------------------------------------------------------------------------

Game::Game()
{
	// assume que a instância da engine é  
	// criada antes da instância do jogo

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