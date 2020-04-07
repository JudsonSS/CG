/**********************************************************************************
// DXGI_DXUT (C�digo Fonte)
//
// Cria��o:		19 Jan 2020
// Atualiza��o:	02 Fev 2020
// Compilador:	Visual C++ 2019
//
// Descri��o:	Testa a infraestrutura gr�fica do DirectX
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
	// aplica��o pausa/resume ao perder/ganhar o foco do usu�rio
	window->LostFocus(Engine::Pause);
	window->InFocus(Engine::Resume);
}

// ------------------------------------------------------------------------------

void DXApp::Update()
{
	// sai com o pressionamento da tecla ESC
	if (input->KeyCtrl(VK_ESCAPE))
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

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	try
	{
		// cria motor e configura a janela
		Engine* engine = new Engine();
		engine->window->Mode(WINDOWED);
		engine->window->Size(1024, 600);
		engine->window->Color(0, 122, 204);
		engine->window->Title("DXGI DXUT");
		engine->window->Icon(IDI_ICON);
		engine->window->Cursor(IDC_CURSOR);

		// cria e executa a aplica��o
		int exitCode = engine->Start(new DXApp());

		// finaliza execu��o
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

