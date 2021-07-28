/**********************************************************************************
// Input (Código Fonte)
//
// Criação:		06 Jan 2020
// Atualização:	27 Jul 2021
// Compilador:	Visual C++ 2019
//
// Descrição:	A classe Input concentra todas as tarefas relacionadas
//              aos dispositivos de entrada do tipo teclado e mouse.
//
**********************************************************************************/

#include "Input.h"

// -------------------------------------------------------------------------------
// inicialização de membros estáticos da classe

bool Input::keys[256] = { 0 };			// estado do teclado/mouse
bool Input::ctrl[256] = { 0 };			// controle de liberação das teclas
									
// -------------------------------------------------------------------------------

Input::Input()
{
	// ATENÇÂO: supõe que a janela já foi criada com uma chamada a window->Create();
	// altera a window procedure da janela ativa para InputProc
	SetWindowLongPtr(GetActiveWindow(), GWLP_WNDPROC, (LONG_PTR)Input::InputProc);
}

// -------------------------------------------------------------------------------

Input::~Input()
{
	// volta a usar a Window Procedure da classe Window
	SetWindowLongPtr(GetActiveWindow(), GWLP_WNDPROC, (LONG_PTR)Window::WinProc);
}

// -------------------------------------------------------------------------------

bool Input::KeyPress(int vkcode)
{
	if (ctrl[vkcode])
	{
		if (KeyDown(vkcode))
		{
			ctrl[vkcode] = false;
			return true;
		}
	}
	else if (KeyUp(vkcode))
	{
		ctrl[vkcode] = true;
	}

	return false;
}

// -------------------------------------------------------------------------------

LRESULT CALLBACK Input::InputProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// tecla pressionada
	case WM_KEYDOWN:
		keys[wParam] = true;
		return 0;

		// tecla liberada
	case WM_KEYUP:
		keys[wParam] = false;
		return 0;
	}

	return CallWindowProc(Window::WinProc, hWnd, msg, wParam, lParam);
}

// -------------------------------------------------------------------------------
