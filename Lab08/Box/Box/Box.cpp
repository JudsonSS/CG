/**********************************************************************************
// Box (Código Fonte)
//
// Criação:		13 Fev 2020
// Atualização:	13 Fev 2020
// Compilador:	Visual C++ 2019
//
// Descrição:	Constrói um cubo 3D usando o Direct3D 12
//
**********************************************************************************/

#include "Box.h"

// ------------------------------------------------------------------------------

void Box::Init()
{
	// aplicação pausa/resume ao perder/ganhar o foco do usuário
	window->LostFocus(Engine::Pause);
	window->InFocus(Engine::Resume);
}

// ------------------------------------------------------------------------------

void Box::Update()
{
	// sai com o pressionamento da tecla ESC
	if (input->KeyCtrl(VK_ESCAPE))
		window->Close();
}

// ------------------------------------------------------------------------------

void Box::Draw()
{
	graphics->Clear(nullptr);

	// submete comandos de desenho aqui
	// ...

	graphics->Present();	
}

// ------------------------------------------------------------------------------

void Box::Finalize()
{
}


// ------------------------------------------------------------------------------
//                                     D3D                                      
// ------------------------------------------------------------------------------

void Box::BuildInputLayout()
{
	// define layout de entrada dos vértices no pipeline
	inputLayout[0] = 
	{ 
		"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 
	};
	
	inputLayout[1] = 
	{ 
		"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, 
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 
	};
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
		engine->window->Title("Box");
		engine->window->Icon(IDI_ICON);
		engine->window->Cursor(IDC_CURSOR);

		// cria e executa a aplicação
		int exitCode = engine->Start(new Box());

		// finaliza execução
		delete engine;
		return exitCode;
	}
	catch (Error & e)
	{
		// exibe mensagem em caso de erro
		MessageBox(nullptr, e.ToString().data(), "Box", MB_OK);
		return 0;
	}
}

// ----------------------------------------------------------------------------

