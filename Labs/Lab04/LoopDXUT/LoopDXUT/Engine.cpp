/**********************************************************************************
// Engine (C�digo Fonte)
//
// Cria��o:		11 Jan 2020
// Atualiza��o:	15 Jan 2020
// Compilador:	Visual C++ 2019
//
// Descri��o:	A Engine roda aplica��es criadas a partir da classe App.
//
//              Para usar a Engine crie uma inst�ncia e chame o m�todo
//				Start() com um objeto derivado da classe App.
//
**********************************************************************************/

#include "Engine.h"
#include <windows.h>
#include <sstream>
using std::stringstream;

// ------------------------------------------------------------------------------
// Inicializa��o de vari�veis est�ticas da classe

Window* Engine::window = nullptr;		// janela da aplica��o
Input* Engine::input = nullptr;			// dispositivos de entrada
App* Engine::app = nullptr;				// apontadador da aplica��o

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

	// cria janela da aplica��o
	window->Create();

	// ATEN��O: a entrada deve ser instanciada ap�s a cria��o da janela
	input = new Input();

	// altera a window procedure da janela ativa para EngineProc
	SetWindowLongPtr(window->Id(), GWLP_WNDPROC, (LONG_PTR)EngineProc);

	// retorna resultado da execu��o
	return Loop();
}

// -------------------------------------------------------------------------------

int Engine::Loop()
{
	// mensagens do Windows
	MSG msg = { 0 };
	
	// inicializa��o da aplica��o
	app->Init();

	// la�o principal
	do
	{
		// trata todos os eventos antes de atualizar a aplica��o
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// atualiza��o da aplica��o 
			app->Update();

			// desenho da aplica��o
			app->Draw();

			// aguarda 16 milissegundos ou a pr�xima intera��o com a janela
			MsgWaitForMultipleObjects(0, NULL, FALSE, 16, QS_ALLINPUT);
		}

	} while (msg.message != WM_QUIT);

	// finaliza��o do aplica��o
	app->Finalize();	

	// encerra aplica��o
	return int(msg.wParam);
}

// -------------------------------------------------------------------------------

LRESULT CALLBACK Engine::EngineProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc;      // representa o dispositivo gr�fico
	PAINTSTRUCT ps;       // guarda regi�o invalidada da janela

	switch (msg)
	{
	// janela precisa ser repintada
	case WM_PAINT:
		// sinaliza	para a janela que a pintura foi feita
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);

		// chama a fun��o de exibi��o da aplica��o
		app->Display();
		return 0;
	}

	return CallWindowProc(Input::InputProc, hWnd, msg, wParam, lParam);
}

// -----------------------------------------------------------------------------
