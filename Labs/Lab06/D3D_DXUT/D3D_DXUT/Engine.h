/**********************************************************************************
// Engine (Arquivo de Cabe�alho)
//
// Cria��o:     15 Mai 2014
// Atualiza��o: 07 Ago 2021
// Compilador:  Visual C++ 2019
//
// Descri��o:   A Engine roda aplica��es criadas a partir da classe App. 
//                
//              Para usar a Engine crie uma inst�ncia e chame o m�todo
//              Start() com um objeto derivado da classe App.
//
**********************************************************************************/

#ifndef DXUT_ENGINE_H
#define DXUT_ENGINE_H

// ---------------------------------------------------------------------------------

#include "Graphics.h"                   // dispositivo gr�fico
#include "Window.h"                     // janela da aplica��o
#include "Input.h"                      // dispositivo de entrada
#include "Timer.h"                      // medidor de tempo
#include "App.h"                        // aplica��o gr�fica

// ---------------------------------------------------------------------------------

class Engine
{
private:
    static Timer timer;                 // medidor de tempo
    static bool  paused;                // estado do aplica��o

    double FrameTime();                 // calcula o tempo do quadro
    int Loop();                         // inicia la�o principal do motor

public:
    static Graphics* graphics;          // dispositivo gr�fico
    static Window*   window;            // janela da aplica��o
    static Input*    input;             // entrada da aplica��o
    static App*      app;               // aplica��o a ser executada
    static double    frameTime;         // tempo do quadro atual

    Engine();                           // construtor
    ~Engine();                          // destrutor

    int Start(App * application);       // inicia o execu��o da aplica��o
    
    static void Pause();                // pausa o motor
    static void Resume();               // reinicia o motor

    // trata eventos do Windows
    static LRESULT CALLBACK EngineProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

// -----------------------------------------------------------------------------
// Fun��es Inline

inline void Engine::Pause()
{ paused = true; timer.Stop(); }

inline void Engine::Resume()
{ paused = false; timer.Start(); }

// ---------------------------------------------------------------------------------

#endif