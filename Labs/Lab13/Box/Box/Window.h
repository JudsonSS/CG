/**********************************************************************************
// Window (Arquivo de Cabe�alho)
// 
// Cria��o:		19 Mai 2007
// Atualiza��o:	17 Abr 2016
// Compilador:	Visual C++ 14
//
// Descri��o:	A classe abstrai todos os detalhes de configura��o de
//              uma janela para um jogo. 
//
**********************************************************************************/

#ifndef _DX12_WINDOW_H_
#define _DX12_WINDOW_H_

// ---------------------------------------------------------------------------------
// Inclus�es

#include <windows.h>	// inclui fun��es do windows
#include <windowsx.h>	// inclui extens�es do windows
#include "Types.h"      // tipos personalizados do motor
#include <string>		// include a classe string
using std::string;		// permite usar o tipo string sem std::

// ---------------------------------------------------------------------------------
// Constantes globais e enumera��es

enum WindowModes {FULLSCREEN, WINDOWED};

// ---------------------------------------------------------------------------------

class Window
{
private:
	HWND		windowId;			// identificador da janela
	int			windowWidth;		// largura da janela
	int			windowHeight;		// altura da janela
	HICON		windowIcon;			// �cone da janela
	HCURSOR		windowCursor;		// cursor da janela
	COLORREF	windowColor;		// cor de fundo da janela
	string		windowTitle;		// nome da barra de t�tulo
	DWORD		windowStyle;		// estilo da janela 
	int			windowMode;			// modo tela cheia, em janela ou sem borda
	int			windowPosX;			// posi��o inicial da janela no eixo x
	int			windowPosY;			// posi��o inicial da janela no eixo y
	int			windowCenterX;		// centro da janela no eixo x
	int			windowCenterY;		// centro da janela no eixo y
	
public:
	Window();												// construtor

	HWND Id();												// retorna o identificador da janela
	int Width(); 											// retorna a largura atual da janela
	int Height(); 											// retorna a altura atual da janela
	int  Mode() const; 										// retorna o modo atual da janela (FULLSCREEN/WINDOWED)
	int  CenterX() const;									// retorna o centro da janela no eixo x
	int  CenterY() const;									// retorna o centro da janela no eixo y
	string Title() const;									// retorna t�tulo da janela
	COLORREF Color();										// retorna a cor de fundo da janela

	void Icon(const uint icon);								// define o �cone da janela
	void Cursor(const uint cursor);							// define o cursor da janela
	void Title(const string title);							// define o t�tulo da janela 
	void Size(int width, int height);						// define o tamanho (largura e altura) da janela
	void Mode(int mode);									// define o modo da janela (FULLSCREEN/WINDOWED)
	void Color(int r, int g, int b);						// define a cor de fundo da janela	

	void Print(string text, int x, int y, COLORREF color);	// mostra texto na janela
	void ShowCursor(bool show);								// habilita ou desabilita a exbi��o do cursor	
	
	bool Create();											// cria a janela com os valores dos atributos
	void Close();											// fecha a janela e sai do jogo
};

// ---------------------------------------------------------------------------------

// Fun��es Membro Inline

// retorna o identificador da janela do jogo
inline HWND Window::Id()
{ return windowId; }

// retorna a largura atual da janela
inline int Window::Width() 
{ return windowWidth;  }

// retorna a altura atual da janela
inline int Window::Height() 
{ return windowHeight; }

// retorna o modo atual da janela (FULLSCREEN/WINDOWED)
inline int Window::Mode() const
{ return windowMode; }

// retorna o centro da janela no eixo horizontal
inline int Window::CenterX() const
{ return windowCenterX; }

// retorna o centro da janela no eixo vertical
inline int Window::CenterY() const
{ return windowCenterY; }

// retorna t�tulo da janela
inline string Window::Title() const
{ return windowTitle; }

// retorna a cor de fundo da janela
inline COLORREF Window::Color()
{ return windowColor; }

// ----------------------------------------------------------

// define o �cone da janela
inline void Window::Icon(const uint icon)	
{ windowIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(icon)); }

// define o cursor da janela
inline void Window::Cursor(const uint cursor)
{ windowCursor = LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(cursor)); }

// define o t�tulo da janela 
inline void Window::Title(const string title)
{ windowTitle = title; }

// define a cor de fundo da janela
inline void Window::Color(int r, int g, int b)	
{ windowColor = RGB(r,g,b); }

// ----------------------------------------------------------

// habilita ou desabilita a exbi��o do cursor
inline void Window::ShowCursor(bool show)
{ ShowCursor(show); }

// ----------------------------------------------------------

// fecha a janela e sai do jogo 
inline void Window::Close()
{ PostMessage(windowId, WM_DESTROY, 0, 0); }

// ---------------------------------------------------------------------------------

#endif