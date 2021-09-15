/**********************************************************************************
// Engine (Arquivo de Cabe�alho)
//
// Cria��o:		15 Mai 2014
// Atualiza��o:	27 Abr 2016
// Compilador:	Visual C++ 14
//
// Descri��o:	A fun��o da Engine � rodar jogos criados a partir da classe 
//              abstrata Game. Todo jogo deve ser uma classe derivada de Game 
//              e portanto deve implementar as fun��es membro Init, Update, Draw 
//              e Finalize, que ser�o chamadas pelo motor em um loop de tempo real. 
//              Para usar a classe Engine, o programador deve criar uma inst�ncia 
//              e chamar o m�todo Start(), passando um objeto derivado de Game.
//
**********************************************************************************/

#ifndef _DX12_ENGINE_H_
#define _DX12_ENGINE_H_

// ---------------------------------------------------------------------------------

#include "Game.h"					// l�gica do jogo
#include "Window.h"					// janela do jogo
#include "Input.h"					// dispositivo de entrada
#include "Graphics.h"				// dispositivo gr�fico
#include "Timer.h"					// medidor de tempo

// ---------------------------------------------------------------------------------

class Engine
{
private:
	Game * game;					// jogo em execu��o
	Timer  timer;					// medidor de tempo
	bool   paused;					// estado da engine

	float  FrameTime();				// calcula o tempo do quadro
	int    Loop();					// loop de tempo real do jogo

public:
	static Window   * window;		// janela
	static Input    * input;		// dispositivos de entrada
	static Graphics * graphics;		// dispositivo gr�fico
	static float      gameTime;		// tempo do frame

	Engine();						// construtor
	~Engine();						// destrutor

	int Start(Game * game);			// inicia o execu��o do jogo
};

// ---------------------------------------------------------------------------------

#endif