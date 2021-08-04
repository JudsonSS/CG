/**********************************************************************************
// App (Código Fonte)
//
// Criação:     11 Jan 2020
// Atualização: 04 Ago 2021
// Compilador:  Visual C++ 2019
//
// Descrição:   Uma classe abstrata para representar uma aplicação
//
//              Todas as aplicações derivadas da classe App devem
//              sobrescrever os métodos Init, Update, Draw e Finalize.
//
**********************************************************************************/

#include "App.h"
#include "Engine.h"

// -------------------------------------------------------------------------------
// Inicialização de membros estáticos da classe

// As referências fazem com que não seja necessário verificar
// se os ponteiros de Engine foram inicializados. Sendo uma
// referência, os atributos de App serão atualizados tão logo
// os atributos de Engine recebam valores

Graphics*& App::graphics = Engine::graphics;    // componente gráfico 
Window*& App::window = Engine::window;          // janela da aplicação
Input*& App::input = Engine::input;             // dispositivos de entrada
double& App::frameTime = Engine::frameTime;     // tempo do último quadro

// -------------------------------------------------------------------------------

App::App()
{
}

// -------------------------------------------------------------------------------

App::~App()
{
}

// -------------------------------------------------------------------------------