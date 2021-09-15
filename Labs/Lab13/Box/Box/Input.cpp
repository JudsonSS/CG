/**********************************************************************************
// Input (Código Fonte)
//
// Criação:		17 Abr 2016
// Atualização:	27 Abr 2016
// Compilador:	Visual C++ 14
//
// Descrição:	A classe Input concentra todas as tarefas relacionadas aos
//              dispositivos de entrada, seja teclado e mouse ou dispositivos
//              tipo gamepad e joystick.
//
**********************************************************************************/

#include "Input.h"

// -------------------------------------------------------------------------------

// Inicialização de membros estáticos da classe
bool Input::inputKeys[256] = { 0 };					// estado do teclado/mouse
bool Input::inputCtrl[256] = { 0 };					// controle de liberação das teclas
int	 Input::inputMouseX = 0;						// posição do mouse no eixo x
int	 Input::inputMouseY = 0;						// posição do mouse no eixo y
									
// -------------------------------------------------------------------------------

Input::Input()
{
}

// -------------------------------------------------------------------------------

Input::~Input()
{

}

// -------------------------------------------------------------------------------

bool Input::KeyCtrl(int vkcode)
{
	if (inputCtrl[vkcode])
	{
		if (KeyDown(vkcode))
		{
			inputCtrl[vkcode] = false;
			return true;
		}
	}
	else if (KeyUp(vkcode))
	{
		inputCtrl[vkcode] = true;
	}

	return false;
}

// -------------------------------------------------------------------------------

LRESULT CALLBACK Input::Handler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// movimento do mouse
	case WM_MOUSEMOVE:			
		inputMouseX = GET_X_LPARAM(lParam);
		inputMouseY = GET_Y_LPARAM(lParam);
		return 0;

		// tecla pressionada
	case WM_KEYDOWN:			
		inputKeys[wParam] = true;
		return 0;

		// botão esquerdo do mouse pressionado
	case WM_LBUTTONDOWN:		
	case WM_LBUTTONDBLCLK:
		inputKeys[VK_LBUTTON] = true;
		return 0;

		// botão do meio do mouse pressionado
	case WM_MBUTTONDOWN:		
	case WM_MBUTTONDBLCLK:
		inputKeys[VK_MBUTTON] = true;
		return 0;

		// botão direito do mouse pressionado
	case WM_RBUTTONDOWN:		
	case WM_RBUTTONDBLCLK:
		inputKeys[VK_RBUTTON] = true;
		return 0;

		// tecla liberada
	case WM_KEYUP:				
		inputKeys[wParam] = false;
		return 0;

		// botão esquerdo do mouse liberado
	case WM_LBUTTONUP:			
		inputKeys[VK_LBUTTON] = false;
		return 0;

		// botão do meio do mouse liberado
	case WM_MBUTTONUP:			
		inputKeys[VK_MBUTTON] = false;
		return 0;

		// botão direito do mouse liberado
	case WM_RBUTTONUP:			
		inputKeys[VK_RBUTTON] = false;
		return 0;
		
		// intercepta ALT-F4 
	//case WM_SYSKEYDOWN:			
	//	if (wParam == VK_F4)
	//		return 0;
	//	break;

		// a janela foi destruida
	case WM_DESTROY:
		// envia uma mensagem WM_QUIT para encerrar o loop da aplicação
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// -------------------------------------------------------------------------------
