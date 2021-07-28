/**********************************************************************************
// Input (Arquivo de Cabe�alho)
//
// Cria��o:		06 Jan 2020
// Atualiza��o:	27 Jul 2021
// Compilador:	Visual C++ 2019
//
// Descri��o:	A classe Input concentra todas as tarefas relacionadas 
//              aos dispositivos de entrada do tipo teclado e mouse.
//
**********************************************************************************/

#ifndef DXUT_INPUT_H
#define DXUT_INPUT_H

// ---------------------------------------------------------------------------------

#include "Window.h"

// ---------------------------------------------------------------------------------

class Input
{
private:
	static bool	keys[256];			// estado das teclas do teclado/mouse
	static bool ctrl[256];			// controle da libera��o de teclas

public:
	Input();						// construtor
	~Input();						// destrutor

	bool KeyDown(int vkcode);		// verifica se uma tecla/bot�o est� pressionado
	bool KeyUp(int vkcode);			// verifica se uma tecla/bot�o est� liberado
	bool KeyPress(int vkcode);		// registra pressionamente somente ap�s libera��o		

	static LRESULT CALLBACK InputProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

// ---------------------------------------------------------------------------------

// Fun��es Membro Inline

// retorna verdadeiro se a tecla est� pressionada
inline bool Input::KeyDown(int vkcode)
{ return keys[vkcode]; }

// retorna verdadeiro se a tecla est� liberada
inline bool Input::KeyUp(int vkcode)
{ return !(keys[vkcode]); }

// ---------------------------------------------------------------------------------

#endif
