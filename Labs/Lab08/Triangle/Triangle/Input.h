/**********************************************************************************
// Input (Arquivo de Cabeçalho)
//
// Criação:     06 Jan 2020
// Atualização: 07 Ago 2021
// Compilador:  Visual C++ 2019
//
// Descrição:   A classe Input concentra todas as tarefas relacionadas 
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
    static bool keys[256];              // estado das teclas do teclado/mouse
    static bool ctrl[256];              // controle da liberação de teclas
    static string text;                 // armazenamento para os caracteres

    static int mouseX;                  // posição do mouse eixo x
    static int mouseY;                  // posição do mouse eixo y
    static short mouseWheel;            // valor da roda do mouse

public:
    Input();                            // construtor
    ~Input();                           // destrutor

    bool KeyDown(int vkcode);           // retorna se tecla está pressionada
    bool KeyUp(int vkcode);             // retorna se tecla está liberada
    bool KeyPress(int vkcode);          // novo pressionamento somente após liberação        
    
    int   MouseX();                     // retorna posição x do mouse
    int   MouseY();                     // retorna posição y do mouse
    short MouseWheel();                 // retorna rotação da roda do mouse

    void  Read();                       // armazena texto digitado até o próximo ENTER ou TAB
    static const char* Text();          // retorna endereço do texto armazenada

    // trata eventos do Windows
    static LRESULT CALLBACK Reader(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
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

// retorna a posição do mouse no eixo x
inline int Input::MouseX()
{ return mouseX; }

// retorna a posição do mouse no eixo y
inline int Input::MouseY()
{ return mouseY; }

// retorna conteúdo do texto lido
inline const char* Input::Text()
{ return text.c_str(); }

// ---------------------------------------------------------------------------------

#endif
