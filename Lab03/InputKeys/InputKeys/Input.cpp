/**********************************************************************************
// Input (C�digo Fonte)
//
// Cria��o:		06 Jan 2020
// Atualiza��o:	11 Jan 2020
// Compilador:	Visual C++ 2019
//
// Descri��o:	A classe Input concentra todas as tarefas relacionadas
//              aos dispositivos de entrada do tipo teclado e mouse.
//
**********************************************************************************/

#include "Input.h"

// -------------------------------------------------------------------------------
// inicializa��o de membros est�ticos da classe

bool Input::keys[256] = { 0 };			// estado do teclado/mouse
bool Input::ctrl[256] = { 0 };			// controle de libera��o das teclas
									
// -------------------------------------------------------------------------------

Input::Input()
{
	// ATEN��O: sup�e que a janela j� foi criada com uma chamada a window->Create();
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

bool Input::KeyCtrl(int vkcode)
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
