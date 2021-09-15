/**********************************************************************************
// Engine (Arquivo de Cabeçalho)
//
// Criação:		15 Mai 2014
// Atualização:	27 Abr 2016
// Compilador:	Visual C++ 14
//
// Descrição:	A função da Engine é rodar jogos criados a partir da classe 
//              abstrata Game. Todo jogo deve ser uma classe derivada de Game 
//              e portanto deve implementar as funções membro Init, Update, Draw 
//              e Finalize, que serão chamadas pelo motor em um loop de tempo real. 
//              Para usar a classe Engine, o programador deve criar uma instância 
//              e chamar o método Start(), passando um objeto derivado de Game.
//
**********************************************************************************/

#ifndef _DX12_ENGINE_H_
#define _DX12_ENGINE_H_

// ---------------------------------------------------------------------------------

#include "Game.h"					// lógica do jogo
#include "Window.h"					// janela do jogo
#include "Input.h"					// dispositivo de entrada
#include "Graphics.h"				// dispositivo gráfico
#include "Timer.h"					// medidor de tempo

// ---------------------------------------------------------------------------------

class Engine
{
private:
	Game * game;					// jogo em execução
	Timer  timer;					// medidor de tempo
	bool   paused;					// estado da engine

	float  FrameTime();				// calcula o tempo do quadro
	int    Loop();					// loop de tempo real do jogo

public:
	static Window   * window;		// janela
	static Input    * input;		// dispositivos de entrada
	static Graphics * graphics;		// dispositivo gráfico
	static float      gameTime;		// tempo do frame

	Engine();						// construtor
	~Engine();						// destrutor

	int Start(Game * game);			// inicia o execução do jogo
};

// ---------------------------------------------------------------------------------

#endif