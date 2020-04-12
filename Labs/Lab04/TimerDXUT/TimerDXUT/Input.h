/**********************************************************************************
// Input (Arquivo de Cabe�alho)
//
// Cria��o:		17 Abr 2016
// Atualiza��o:	11 Jan 2020
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
	static bool	 keys[256];				// estado das teclas do teclado/mouse
	static bool  ctrl[256];				// controle da libera��o de teclas
	static int	 mouseX;					// posi��o do mouse eixo x
	static int	 mouseY;					// posi��o do mouse eixo y
	static short mouseWheel;			// valor da roda do mouse

	static const uint textLimit = 80;	// tamanho m�ximo do texto
	static uint  textIndex;				// posi��o atual de inser��o no texto
	static char  text[textLimit];		// armazenamento para os caracteres digitados

public:
	Input();							// construtor
	~Input();							// destrutor

	bool  KeyDown(int vkcode);			// verifica se uma tecla/bot�o est� pressionado
	bool  KeyUp(int vkcode);				// verifica se uma tecla/bot�o est� liberado
	bool  KeyCtrl(int vkcode);			// registra pressionamente somente ap�s libera��o		
	
	int   MouseX();						// retorna posi��o x do mouse
	int   MouseY();						// retorna posi��o y do mouse
	short MouseWheel();					// retorna rota��o da roda do mouse

	void  Read();						// armazena texto digitado at� o pr�ximo ENTER ou TAB
	static char* Text();				// retorna endere�o do texto armazenada

	// trata eventos do Windows
	static LRESULT CALLBACK Reader(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
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

// retorna a posi��o do mouse no eixo x
inline int Input::MouseX()
{ return mouseX; }

// retorna a posi��o do mouse no eixo y
inline int Input::MouseY()
{ return mouseY; }

// retorna conte�do do texto lido
inline char* Input::Text()
{ return text; }

// ---------------------------------------------------------------------------------

#endif
