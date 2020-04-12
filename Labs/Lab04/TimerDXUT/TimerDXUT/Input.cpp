/**********************************************************************************
// Input (C�digo Fonte)
//
// Cria��o:		17 Abr 2016
// Atualiza��o:	17 Jan 2020
// Compilador:	Visual C++ 2019
//
// Descri��o:	A classe Input concentra todas as tarefas relacionadas
//              aos dispositivos de entrada do tipo teclado e mouse.
//
**********************************************************************************/

#include "Input.h"

// -------------------------------------------------------------------------------
// inicializa��o de membros est�ticos da classe

bool  Input::keys[256] = { 0 };						// estado do teclado/mouse
bool  Input::ctrl[256] = { 0 };						// controle de libera��o das teclas
int	  Input::mouseX = 0;								// posi��o do mouse no eixo x
int	  Input::mouseY = 0;								// posi��o do mouse no eixo y
short Input::mouseWheel = 0;							// valor da roda do mouse

uint Input::textIndex = 0;							// posi��o atual de inser��o em "text"
char Input::text[textLimit] = { 0 };				// guarda caracteres digitados
									
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

short Input::MouseWheel()
{
	short val = mouseWheel;
	mouseWheel = 0;
	return val;
}

// -------------------------------------------------------------------------------

void Input::Read()
{
	// apaga texto armazenado
	textIndex = 0;
	ZeroMemory(text, textLimit);

	// altera a window procedure da janela ativa
	SetWindowLongPtr(GetActiveWindow(), GWLP_WNDPROC, (LONG_PTR)Input::Reader);
}

// -------------------------------------------------------------------------------

LRESULT CALLBACK Input::Reader(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	// processa teclas de caracteres
	case WM_CHAR:
		switch (wParam)
		{
		// Backspace
		case 0x08:
			if (textIndex > 0)
			{
				textIndex--;
				text[textIndex] = '\0';
			}
			break;

		// Tab e Enter
		case 0x09:
		case 0x0D:
			// altera a window procedure da janela ativa
			SetWindowLongPtr(GetActiveWindow(), GWLP_WNDPROC, (LONG_PTR)Input::InputProc);
			break;

		// Caracteres
		default:
			if (textIndex < textLimit)
				text[textIndex++] = char(wParam);
			break;
		}
		// ATEN��O: n�o ser� necess�rio quando estiver operando com DirectX
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	}

	return CallWindowProc(Input::InputProc, hWnd, msg, wParam, lParam);
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
		
	// movimento do mouse
	case WM_MOUSEMOVE:			
		mouseX = GET_X_LPARAM(lParam);
		mouseY = GET_Y_LPARAM(lParam);
		return 0;

	// movimento da roda do mouse
	case WM_MOUSEWHEEL:
		mouseWheel = GET_WHEEL_DELTA_WPARAM(wParam);
		return 0;

	// bot�o esquerdo do mouse pressionado
	case WM_LBUTTONDOWN:		
	case WM_LBUTTONDBLCLK:
		keys[VK_LBUTTON] = true;
		return 0;

	// bot�o do meio do mouse pressionado
	case WM_MBUTTONDOWN:		
	case WM_MBUTTONDBLCLK:
		keys[VK_MBUTTON] = true;
		return 0;

	// bot�o direito do mouse pressionado
	case WM_RBUTTONDOWN:		
	case WM_RBUTTONDBLCLK:
		keys[VK_RBUTTON] = true;
		return 0;

	// bot�o esquerdo do mouse liberado
	case WM_LBUTTONUP:			
		keys[VK_LBUTTON] = false;
		return 0;

	// bot�o do meio do mouse liberado
	case WM_MBUTTONUP:			
		keys[VK_MBUTTON] = false;
		return 0;

	// bot�o direito do mouse liberado
	case WM_RBUTTONUP:			
		keys[VK_RBUTTON] = false;
		return 0;
	}

	return CallWindowProc(Window::WinProc, hWnd, msg, wParam, lParam);
}

// -------------------------------------------------------------------------------
