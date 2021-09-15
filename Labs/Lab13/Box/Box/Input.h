/**********************************************************************************
// Input (Arquivo de Cabe�alho)
//
// Cria��o:		17 Abr 2016
// Atualiza��o:	27 Abr 2016
// Compilador:	Visual C++ 14
//
// Descri��o:	A classe Input concentra todas as tarefas relacionadas aos
//              dispositivos de entrada, seja teclado e mouse ou dispositivos 
//              tipo gamepad e joystick.
//
**********************************************************************************/

#ifndef _DX12_INPUT_H_
#define _DX12_INPUT_H_

// ---------------------------------------------------------------------------------

#include <windows.h>				// inclui fun��es/tipos do windows
#include <windowsx.h>				// inclui extens�es do windows

// ---------------------------------------------------------------------------------

class Input
{
private:
	static bool	inputKeys[256];		// estado das teclas do teclado/mouse
	static bool inputCtrl[256];		// controle da libera��o de teclas
	static int	inputMouseX;		// posi��o do mouse eixo x
	static int	inputMouseY;		// posi��o do mouse eixo y

public:
	Input();						// construtor
	~Input();						// destrutor

	bool KeyDown(int vkcode);		// verifica se uma tecla/bot�o est� pressionado
	bool KeyUp(int vkcode);			// verifica se uma tecla/bot�o est� liberado
	bool KeyCtrl(int vkcode);		// registra pressionamente somente ap�s libera��o		
	int  MouseX();					// retorna posi��o x do mouse
	int  MouseY();					// retorna posi��o y do mouse

	// trata eventos do Windows
	static LRESULT CALLBACK Handler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

// ---------------------------------------------------------------------------------

// Fun��es Membro Inline

// retorna verdadeiro se a tecla est� pressionada
inline bool Input::KeyDown(int vkcode)
{ return inputKeys[vkcode]; }

// retorna verdadeiro se a tecla est� liberada
inline bool Input::KeyUp(int vkcode)
{ return !(inputKeys[vkcode]); }

// retorna a posi��o do mouse no eixo x
inline int Input::MouseX()
{ return inputMouseX; }

// retorna a posi��o do mouse no eixo y
inline int Input::MouseY()
{ return inputMouseY; }

// ---------------------------------------------------------------------------------

#endif
