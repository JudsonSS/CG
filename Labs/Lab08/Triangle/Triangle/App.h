/**********************************************************************************
// App (Arquivo de Cabeçalho)
// 
// Criação:     11 Jan 2020
// Atualização: 15 Ago 2021
// Compilador:  Visual C++ 2019
//
// Descrição:   Uma classe abstrata para representar uma aplicação
//
//              Todas as aplicações derivadas da classe App devem
//              sobrescrever os métodos Init, Update, Draw e Finalize. 
//
**********************************************************************************/

#ifndef DXUT_APP_H
#define DXUT_APP_H

// ---------------------------------------------------------------------------------

#include "Graphics.h"
#include "Window.h"
#include "Input.h"

// ---------------------------------------------------------------------------------

class App
{
    // Membros protegidos são privados para o mundo externo mas
    // públicos para as classes derivadas, ou seja, as classes
    // derivadas de App serão capazes de acessar estes membros.

protected:
    static Graphics* & graphics;                // componente gráfico
    static Window*   & window;                  // janela da aplicação
    static Input*    & input;                   // dispositivos de entrada
    static double    & frameTime;               // tempo do último quadro

public:
    App();                                      // construtor
    virtual ~App();                             // destrutor

    // Estes métodos são puramente virtuais, isto é, devem ser 
    // implementados em todas as classes derivas de App.

    virtual void Init() = 0;                    // inicialização
    virtual void Update() = 0;                  // atualização
    virtual void Finalize() = 0;                // finalização    

    // Estes métodos possuem uma implementação vazia por padrão
    // e apenas um deles deve ser sobrescrito na classe derivada:
    // - Draw é executado a cada ciclo do laço principal
    // - Display é chamado apenas uma vez no início da aplicação
    //   e deve ser chamado manualmente em Update toda vez
    //   que a tela precisar ser redesenhada.

    virtual void Draw() {}                      // desenho
    virtual void Display() {}                   // exibição
    virtual void OnPause() { Sleep(10); }       // em pausa
};

// ---------------------------------------------------------------------------------

#endif