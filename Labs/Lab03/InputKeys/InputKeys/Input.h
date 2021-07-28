/**********************************************************************************
// Input (Arquivo de Cabeçalho)
//
// Criação:		06 Jan 2020
// Atualização:	27 Jul 2021
// Compilador:	Visual C++ 2019
//
// Descrição:	A classe Input concentra todas as tarefas relacionadas 
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
	static bool ctrl[256];			// controle da liberação de teclas

public:
	Input();						// construtor
	~Input();						// destrutor

	bool KeyDown(int vkcode);		// verifica se uma tecla/botão está pressionado
	bool KeyUp(int vkcode);			// verifica se uma tecla/botão está liberado
	bool KeyPress(int vkcode);		// registra pressionamente somente após liberação		

	static LRESULT CALLBACK InputProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

// ---------------------------------------------------------------------------------

// Funções Membro Inline

// retorna verdadeiro se a tecla está pressionada
inline bool Input::KeyDown(int vkcode)
{ return keys[vkcode]; }

// retorna verdadeiro se a tecla está liberada
inline bool Input::KeyUp(int vkcode)
{ return !(keys[vkcode]); }

// ---------------------------------------------------------------------------------

#endif
