/**********************************************************************************
// Engine (C�digo Fonte)
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

#include "Engine.h"
#include <windows.h>
#include <sstream>
using std::stringstream;

// -------------------------------------------------------------------------------
// Inicializa��o de membros est�ticos da classe

Window   * Engine::window   = nullptr;	// janela do jogo
Input    * Engine::input    = nullptr;	// dispositivo de entrada
Graphics * Engine::graphics = nullptr;  // dispositivo gr�fico
float      Engine::gameTime = 0;		// tempo do frame

// -------------------------------------------------------------------------------

Engine::Engine() 
{
	game     = nullptr;
	paused   = false;
	window   = new Window();
	input    = new Input();
	graphics = new Graphics();
}

// -------------------------------------------------------------------------------

Engine::~Engine()
{
	delete graphics;
	delete input;
	delete window;

	if (game)
		delete game;
}

// -----------------------------------------------------------------------------

int Engine::Start(Game * game)
{
	this->game = game;

	// cria janela do jogo
	window->Create();

	// inicializa dispositivo gr�fico
	if (!graphics->Initialize(window))
		return EXIT_FAILURE;

	// retorna resultado da execu��o do jogo
	return Loop();
}

// -------------------------------------------------------------------------------

float Engine::FrameTime()
{
	// tempo transcorrido para o quadro atual
	static float frameTime = 0.0f;	

	// pega tempo transcorrido para o frame atual (em segundos)
	frameTime = timer.Reset();

#ifdef _DEBUG
	static float totalTime = 0.0f;	// tempo total transcorrido 
	static uint  frameCount = 0;	// contador de frames transcorridos

	// tempo acumulado dos frames
	totalTime += frameTime;

	// incrementa contador de frames
	frameCount++;

	// a cada 1000ms atualiza indicador de FPS na janela
	if (totalTime >= 1.000f)
	{
		stringstream text;			// fluxo de texto para mensagens
		text << std::fixed;			// sempre mostra a parte fracion�ria
		text.precision(3);			// tr�s casas depois da v�rgula

		text << window->Title().c_str() << "    "
			<< "FPS: " << int(frameCount / totalTime) << "    "
			<< "Frame Time: " << totalTime * 1000.0f / frameCount << " (ms)";

		SetWindowText(window->Id(), text.str().c_str());

		frameCount = 0;
		totalTime = 0.0f;
	}
#endif 

	return frameTime;
}

// -------------------------------------------------------------------------------

int Engine::Loop()
{
	// mensagens do Windows
	MSG msg = {};

	// inicializa��o do jogo
	game->Init();

	// inicia contagem de tempo
	timer.Start();

	// loop principal do jogo
	do
	{
		// testa se tem mensagem do windows para tratar
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// -----------------------------------------------
			// Pausa/Resume Jogo
			// -----------------------------------------------

			if (input->KeyCtrl('P'))
			{
				paused = !paused;

				if (paused)
					timer.Stop();
				else
					timer.Start();
			}

			// -----------------------------------------------

			if (!paused)
			{
				// tempo gasto no processamento do �ltimo frame
				gameTime = FrameTime();

				// atualiza��o do jogo 
				game->Update();

				// desenha o jogo
				game->Draw();
			} 
			else
			{ 
				// durma um pouco porque o jogo est� parado
				Sleep(15);
			}
		}

	} while (msg.message != WM_QUIT);

	// finaliza��o do jogo
	game->Finalize();

	// encerra aplica��o
	return int(msg.wParam);
}

// -----------------------------------------------------------------------------
