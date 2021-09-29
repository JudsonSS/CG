/**********************************************************************************
// Engine (Arquivo de Cabeçalho)
//
// Criação:     15 Mai 2014
// Atualização: 07 Ago 2021
// Compilador:  Visual C++ 2019
//
// Descrição:   A Engine roda aplicações criadas a partir da classe App. 
//                
//              Para usar a Engine crie uma instância e chame o método
//              Start() com um objeto derivado da classe App.
//
**********************************************************************************/

#ifndef DXUT_ENGINE_H
#define DXUT_ENGINE_H

// ---------------------------------------------------------------------------------

#include "Graphics.h"                   // dispositivo gráfico
#include "Window.h"                     // janela da aplicação
#include "Input.h"                      // dispositivo de entrada
#include "Timer.h"                      // medidor de tempo
#include "App.h"                        // aplicação gráfica

// ---------------------------------------------------------------------------------

class Engine
{
private:
    static Timer timer;                 // medidor de tempo
    static bool  paused;                // estado do aplicação

    double FrameTime();                 // calcula o tempo do quadro
    int Loop();                         // inicia laço principal do motor

public:
    static Graphics* graphics;          // dispositivo gráfico
    static Window*   window;            // janela da aplicação
    static Input*    input;             // entrada da aplicação
    static App*      app;               // aplicação a ser executada
    static double    frameTime;         // tempo do quadro atual

    Engine();                           // construtor
    ~Engine();                          // destrutor

    int Start(App * application);       // inicia o execução da aplicação
    
    static void Pause();                // pausa o motor
    static void Resume();               // reinicia o motor

    // trata eventos do Windows
    static LRESULT CALLBACK EngineProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

// -----------------------------------------------------------------------------
// Funções Inline

inline void Engine::Pause()
{ paused = true; timer.Stop(); }

inline void Engine::Resume()
{ paused = false; timer.Start(); }

// ---------------------------------------------------------------------------------

#endif