/**********************************************************************************
// Input (Arquivo de Cabeçalho)
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

#ifndef _DX12_INPUT_H_
#define _DX12_INPUT_H_

// ---------------------------------------------------------------------------------

#include <windows.h>				// inclui funções/tipos do windows
#include <windowsx.h>				// inclui extensões do windows

// ---------------------------------------------------------------------------------

class Input
{
private:
	static bool	inputKeys[256];		// estado das teclas do teclado/mouse
	static bool inputCtrl[256];		// controle da liberação de teclas
	static int	inputMouseX;		// posição do mouse eixo x
	static int	inputMouseY;		// posição do mouse eixo y

public:
	Input();						// construtor
	~Input();						// destrutor

	bool KeyDown(int vkcode);		// verifica se uma tecla/botão está pressionado
	bool KeyUp(int vkcode);			// verifica se uma tecla/botão está liberado
	bool KeyCtrl(int vkcode);		// registra pressionamente somente após liberação		
	int  MouseX();					// retorna posição x do mouse
	int  MouseY();					// retorna posição y do mouse

	// trata eventos do Windows
	static LRESULT CALLBACK Handler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

// ---------------------------------------------------------------------------------

// Funções Membro Inline

// retorna verdadeiro se a tecla está pressionada
inline bool Input::KeyDown(int vkcode)
{ return inputKeys[vkcode]; }

// retorna verdadeiro se a tecla está liberada
inline bool Input::KeyUp(int vkcode)
{ return !(inputKeys[vkcode]); }

// retorna a posição do mouse no eixo x
inline int Input::MouseX()
{ return inputMouseX; }

// retorna a posição do mouse no eixo y
inline int Input::MouseY()
{ return inputMouseY; }

// ---------------------------------------------------------------------------------

#endif
