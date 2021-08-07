/**********************************************************************************
// D3D_DXUT (Código Fonte)
//
// Criação:     19 Jan 2020
// Atualização: 07 Ago 2021
// Compilador:  Visual C++ 2019
//
// Descrição:   Testa a inicialização do D3D12 feita pelo DXUT
//
**********************************************************************************/

#include "DXUT.h"

// ------------------------------------------------------------------------------

class D3DApp : public App
{
public:
    void Init();
    void Update();
    void Draw();
    void Finalize();
};

// ------------------------------------------------------------------------------

void D3DApp::Init()
{ 
}

// ------------------------------------------------------------------------------

void D3DApp::Update()
{
    // sai com o pressionamento da tecla ESC
    if (input->KeyPress(VK_ESCAPE))
        window->Close();
}

// ------------------------------------------------------------------------------

void D3DApp::Draw()
{
    graphics->Clear(nullptr);

    // submete comandos de desenho aqui
    // ...

    graphics->Present();    
}

// ------------------------------------------------------------------------------

void D3DApp::Finalize()
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
        // cria motor e configura a janela
        Engine* engine = new Engine();
        engine->window->Mode(WINDOWED);
        engine->window->Size(1024, 600);
        engine->window->Color(0, 122, 204);
        engine->window->Title("D3D DXUT");
        engine->window->Icon(IDI_ICON);
        engine->window->Cursor(IDC_CURSOR);
        engine->window->LostFocus(Engine::Pause);
        engine->window->InFocus(Engine::Resume);

        // cria e executa a aplicação
        int exitCode = engine->Start(new D3DApp());

        // finaliza execução
        delete engine;
        return exitCode;
    }
    catch (Error & e)
    {
        MessageBox(nullptr, e.ToString().data(), "D3D DXUT", MB_OK);
        return 0;
    }
}

// ----------------------------------------------------------------------------

