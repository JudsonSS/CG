/**********************************************************************************
// DXGI_DXUT (Código Fonte)
//
// Criação:     19 Jan 2020
// Atualização: 04 Ago 2021
// Compilador:  Visual C++ 2019
//
// Descrição:   Testa a infraestrutura gráfica do DirectX
//
**********************************************************************************/

#include "DXUT.h"

// ------------------------------------------------------------------------------

class DXApp : public App
{
public:
    void Init();
    void Update();
    void Draw();
    void Finalize();
};

// ------------------------------------------------------------------------------

void DXApp::Init()
{

}

// ------------------------------------------------------------------------------

void DXApp::Update()
{
    // sai com o pressionamento da tecla ESC
    if (input->KeyPress(VK_ESCAPE))
        window->Close();
}

// ------------------------------------------------------------------------------

void DXApp::Draw()
{
    
}

// ------------------------------------------------------------------------------

void DXApp::Finalize()
{
}


// ------------------------------------------------------------------------------
//                                  WinMain                                      
// ------------------------------------------------------------------------------

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
                     _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    try
    {
        // cria motor
        Engine* engine = new Engine();

        // configura a janela
        engine->window->Mode(WINDOWED);
        engine->window->Size(1024, 600);
        engine->window->Color(0, 122, 204);
        engine->window->Title("DXGI DXUT");
        engine->window->Icon(IDI_ICON);
        engine->window->Cursor(IDC_CURSOR);
        
        // aplicação pausa/resume ao perder/ganhar o foco
        engine->window->LostFocus(Engine::Pause);
        engine->window->InFocus(Engine::Resume);

        // cria e executa a aplicação
        int exitCode = engine->Start(new DXApp());

        // finaliza execução
        delete engine;
        return exitCode;
    }
    catch (Error & e)
    {
        MessageBox(nullptr, e.ToString().data(), "DXGI DXUT", MB_OK);
        return 0;
    }
}

// ----------------------------------------------------------------------------

