/**********************************************************************************
// Window (Arquivo de Cabeçalho)
// 
// Criação:     04 Jan 2020
// Atualização: 24 Jul 2021
// Compilador:  Visual C++ 2019
//
// Descrição:   Abstrai os detalhes de configuração de uma janela
//
**********************************************************************************/

#ifndef DXUT_WINDOW_H
#define DXUT_WINDOW_H

// ---------------------------------------------------------------------------------
// Inclusões

#include "Types.h"      // tipos personalizados da biblioteca
#include <windows.h>    // inclui funções do windows
#include <windowsx.h>   // inclui extensões do windows
#include <string>       // inclui a classe string
using std::string;      // permite usar o tipo string sem std::

// ---------------------------------------------------------------------------------
// Constantes globais e enumerações

enum WindowModes {FULLSCREEN, WINDOWED};

// ---------------------------------------------------------------------------------

class Window
{
private:
    HWND		windowId;                                   // identificador da janela
    int			windowWidth;                                // largura da janela
    int			windowHeight;                               // altura da janela
    HICON		windowIcon;                                 // ícone da janela
    HCURSOR		windowCursor;                               // cursor da janela
    COLORREF	windowColor;                                // cor de fundo da janela
    string		windowTitle;                                // nome da barra de título
    DWORD		windowStyle;                                // estilo da janela 
    int			windowMode;                                 // modo tela cheia, em janela ou sem borda
    int			windowPosX;                                 // posição inicial da janela no eixo x
    int			windowPosY;                                 // posição inicial da janela no eixo y
    int			windowCenterX;                              // centro da janela no eixo x
    int			windowCenterY;                              // centro da janela no eixo y
    
public:
    Window();                                               // construtor

    HWND Id() const;                                        // retorna o identificador da janela
    int Width() const;                                      // retorna a largura atual da janela
    int Height() const;                                     // retorna a altura atual da janela
    int Mode() const;                                       // retorna o modo atual da janela (FULLSCREEN/WINDOWED)
    int CenterX() const;                                    // retorna o centro da janela no eixo x
    int CenterY() const;                                    // retorna o centro da janela no eixo y
    string Title() const;                                   // retorna título da janela
    COLORREF Color() const;                                 // retorna a cor de fundo da janela

    void Icon(const uint icon);                             // define o ícone da janela
    void Cursor(const uint cursor);                         // define o cursor da janela
    void Title(const string title);                         // define o título da janela 
    void Size(int width, int height);                       // define o tamanho (largura e altura) da janela
    void Mode(int mode);                                    // define o modo da janela (FULLSCREEN/WINDOWED)
    void Color(int r, int g, int b);                        // define a cor de fundo da janela	
    
    void HideCursor(bool hide);                             // habilita ou desabilita a exbição do cursor
    void Print(string text, int x, int y, COLORREF color);  // mostra texto na janela	
    void Close();                                           // fecha a janela e sai do jogo
    bool Create();                                          // cria a janela com os valores dos atributos	

    // tratamento de eventos do Windows
    static LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

// ---------------------------------------------------------------------------------

// Funções Membro Inline

// retorna o identificador da janela do jogo
inline HWND Window::Id() const
{ return windowId; }

// retorna a largura atual da janela
inline int Window::Width() const
{ return windowWidth;  }

// retorna a altura atual da janela
inline int Window::Height() const
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

// retorna título da janela
inline string Window::Title() const
{ return windowTitle; }

// retorna a cor de fundo da janela
inline COLORREF Window::Color() const
{ return windowColor; }

// ----------------------------------------------------------

// define o ícone da janela
inline void Window::Icon(const uint icon)	
{ windowIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(icon)); }

// define o cursor da janela
inline void Window::Cursor(const uint cursor)
{ windowCursor = LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(cursor)); }

// define o título da janela 
inline void Window::Title(const string title)
{ windowTitle = title; }

// define a cor de fundo da janela
inline void Window::Color(int r, int g, int b)	
{ windowColor = RGB(r,g,b); }

// ----------------------------------------------------------

// habilita ou desabilita a exbição do cursor
inline void Window::HideCursor(bool hide)
{ ShowCursor(!hide); }

// ----------------------------------------------------------

// fecha a janela e sai do jogo 
inline void Window::Close()
{ PostMessage(windowId, WM_DESTROY, 0, 0); }

// ---------------------------------------------------------------------------------

#endif