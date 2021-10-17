/**********************************************************************************
// Engine (Código Fonte)
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

#include "Engine.h"
#include <windows.h>
#include <sstream>
using std::stringstream;

// ------------------------------------------------------------------------------
// Inicialização de variáveis estáticas da classe

Graphics* Engine::graphics  = nullptr;    // dispositivo gráfico
Window*   Engine::window    = nullptr;    // janela da aplicação
Input*    Engine::input     = nullptr;    // dispositivos de entrada
App*      Engine::app       = nullptr;    // apontadador da aplicação
double    Engine::frameTime = 0.0;        // tempo do quadro atual
bool      Engine::paused    = false;      // estado do motor
Timer     Engine::timer;                  // medidor de tempo

// -------------------------------------------------------------------------------

Engine::Engine()
{
    window = new Window();
    graphics = new Graphics();
}

// -------------------------------------------------------------------------------

Engine::~Engine()
{
    delete app;
    delete graphics;
    delete input;
    delete window;
}

// -----------------------------------------------------------------------------

int Engine::Start(App * application)
{
    app = application;

    // cria janela da aplicação
    window->Create();

    // inicializa dispositivos de entrada (deve ser feito após criação da janela)
    input = new Input();

    // inicializa dispositivo gráfico
    graphics->Initialize(window);

    // altera a window procedure da janela ativa para EngineProc
    SetWindowLongPtr(window->Id(), GWLP_WNDPROC, (LONG_PTR)EngineProc);

    // ajusta a resolução do Sleep para 1 milisegundo
    // requer uso da biblioteca winmm.lib
    timeBeginPeriod(1);

    int exitCode = Loop();

    // volta a resolução do Sleep ao valor original
    timeEndPeriod(1);

    return exitCode;
}

// -----------------------------------------------------------------------------

double Engine::FrameTime()
{

#ifdef _DEBUG
    static double totalTime = 0.0;    // tempo total transcorrido 
    static uint   frameCount = 0;    // contador de frames transcorridos
#endif

    // tempo do frame atual
    frameTime = timer.Reset();

#ifdef _DEBUG
    // tempo acumulado dos frames
    totalTime += frameTime;

    // incrementa contador de frames
    frameCount++;

    // a cada 1000ms (1 segundo) atualiza indicador de FPS na janela
    if (totalTime >= 1.0)
    {
        stringstream text;            // fluxo de texto para mensagens
        text << std::fixed;            // sempre mostra a parte fracionária
        text.precision(3);            // três casas depois da vírgula

        text << window->Title().c_str() << "    "
            << "FPS: " << frameCount << "    "
            << "Frame Time: " << frameTime * 1000 << " (ms)";

        SetWindowText(window->Id(), text.str().c_str());

        frameCount = 0;
        totalTime -= 1.0;
    }
#endif

    return frameTime;
}

// -------------------------------------------------------------------------------

int Engine::Loop()
{
    // inicia contagem de tempo
    timer.Start();
    
    // mensagens do Windows
    MSG msg = { 0 };
    
    // inicialização da aplicação
    app->Init();

    // laço principal
    do
    {
        // trata todos os eventos antes de atualizar a aplicação
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // -----------------------------------------------
            // Pausa/Resume Jogo
            // -----------------------------------------------

            if (input->KeyPress(VK_PAUSE))
            {
                if (paused)
                    Resume();
                else
                    Pause();
            }

            // -----------------------------------------------

            if (!paused)
            {
                // calcula o tempo do quadro
                frameTime = FrameTime();

                // atualização da aplicação 
                app->Update();

                // desenho da aplicação
                app->Draw();
            }
            else
            {
                app->OnPause();
            }
        }

    } while (msg.message != WM_QUIT);

    // finalização do aplicação
    app->Finalize();    

    // encerra aplicação
    return int(msg.wParam);
}

// -------------------------------------------------------------------------------

LRESULT CALLBACK Engine::EngineProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // janela precisa ser repintada
    if (msg == WM_PAINT)
        app->Display();

    return CallWindowProc(Input::InputProc, hWnd, msg, wParam, lParam);
}

// -----------------------------------------------------------------------------
