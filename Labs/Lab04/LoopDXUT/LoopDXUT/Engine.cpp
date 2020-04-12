/**********************************************************************************
// Engine (Código Fonte)
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

#include "Engine.h"
#include <windows.h>
#include <sstream>
using std::stringstream;

// ------------------------------------------------------------------------------
// Inicialização de variáveis estáticas da classe

Window* Engine::window = nullptr;		// janela da aplicação
Input* Engine::input = nullptr;			// dispositivos de entrada
App* Engine::app = nullptr;				// apontadador da aplicação

// -------------------------------------------------------------------------------

Engine::Engine()
{
	window = new Window();
}

// -------------------------------------------------------------------------------

Engine::~Engine()
{
	if (app)
		delete app;

	// volta a usar a Window Procedure da classe Input
	SetWindowLongPtr(window->Id(), GWLP_WNDPROC, (LONG_PTR)Input::InputProc);

	delete input;
	delete window;
}

// -----------------------------------------------------------------------------

int Engine::Start(App * application)
{
	app = application;

	// cria janela da aplicação
	window->Create();

	// ATENÇÂO: a entrada deve ser instanciada após a criação da janela
	input = new Input();

	// altera a window procedure da janela ativa para EngineProc
	SetWindowLongPtr(window->Id(), GWLP_WNDPROC, (LONG_PTR)EngineProc);

	// retorna resultado da execução
	return Loop();
}

// -------------------------------------------------------------------------------

int Engine::Loop()
{
	// mensagens do Windows
	MSG msg = { 0 };
	
	// inicialização da aplicação
	app->Init();

	// laço principal
	do
	{
		// trata todos os eventos antes de atualizar a aplicação
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// atualização da aplicação 
			app->Update();

			// desenho da aplicação
			app->Draw();

			// aguarda 16 milissegundos ou a próxima interação com a janela
			MsgWaitForMultipleObjects(0, NULL, FALSE, 16, QS_ALLINPUT);
		}

	} while (msg.message != WM_QUIT);

	// finalização do aplicação
	app->Finalize();	

	// encerra aplicação
	return int(msg.wParam);
}

// -------------------------------------------------------------------------------

LRESULT CALLBACK Engine::EngineProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc;      // representa o dispositivo gráfico
	PAINTSTRUCT ps;       // guarda região invalidada da janela

	switch (msg)
	{
	// janela precisa ser repintada
	case WM_PAINT:
		// sinaliza	para a janela que a pintura foi feita
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);

		// chama a função de exibição da aplicação
		app->Display();
		return 0;
	}

	return CallWindowProc(Input::InputProc, hWnd, msg, wParam, lParam);
}

// -----------------------------------------------------------------------------
