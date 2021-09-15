/**********************************************************************************
// Engine (Código Fonte)
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

#include "Engine.h"
#include <windows.h>
#include <sstream>
using std::stringstream;

// -------------------------------------------------------------------------------
// Inicialização de membros estáticos da classe

Window   * Engine::window   = nullptr;	// janela do jogo
Input    * Engine::input    = nullptr;	// dispositivo de entrada
Graphics * Engine::graphics = nullptr;  // dispositivo gráfico
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

	// inicializa dispositivo gráfico
	if (!graphics->Initialize(window))
		return EXIT_FAILURE;

	// retorna resultado da execução do jogo
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
		text << std::fixed;			// sempre mostra a parte fracionária
		text.precision(3);			// três casas depois da vírgula

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

	// inicialização do jogo
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
				// tempo gasto no processamento do último frame
				gameTime = FrameTime();

				// atualização do jogo 
				game->Update();

				// desenha o jogo
				game->Draw();
			} 
			else
			{ 
				// durma um pouco porque o jogo está parado
				Sleep(15);
			}
		}

	} while (msg.message != WM_QUIT);

	// finalização do jogo
	game->Finalize();

	// encerra aplicação
	return int(msg.wParam);
}

// -----------------------------------------------------------------------------
