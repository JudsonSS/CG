/**********************************************************************************
// LoopDXUT (Código Fonte)
//
// Criação:		11 Jan 2020
// Atualização:	02 Ago 2021
// Compilador:	Visual C++ 2019
//
// Descrição:	Testa a execução de uma aplicação em um laço de tempo-real
//
**********************************************************************************/

#include "DXUT.h"
#include <sstream>
using std::stringstream;

// ------------------------------------------------------------------------------

class WinApp : public App
{
private:
	stringstream textSize;
	stringstream textMode;
	stringstream textMouse;

	int lastX, lastY;

public:
	void Init();
	void Update();
	void Display();
	void Finalize();	
};

// ------------------------------------------------------------------------------

void WinApp::Init()
{
	// captura posição do mouse
	lastX = input->MouseX();
	lastY = input->MouseY();

	// captura tamanho e tipo de janela
	textSize << "Tamanho: " << window->Width() << " x " << window->Height();
	textMode << "Formato: " << (window->Mode() == WINDOWED ? "Em Janela" : "Tela Cheia");
	textMouse << input->MouseX() << " x " << input->MouseY();
}

// ------------------------------------------------------------------------------

void WinApp::Update()
{
	// sai com o pressionamento da tecla ESC
	if (input->KeyPress(VK_ESCAPE))
		window->Close();

	// converte posição do mouse em texto para exibição
	textMouse.str("");
	textMouse << input->MouseX() << " x " << input->MouseY();

	// se o mouse mudou de posição
	if (lastX != input->MouseX() || lastY != input->MouseY())
	{
		window->Clear();
		Display();
	}	

	// atualiza posição do mouse
	lastX = input->MouseX();
	lastY = input->MouseY();
}

// ------------------------------------------------------------------------------

void WinApp::Display()
{
	window->Print("Window App Demo", 10, 10, RGB(255, 255, 255));
	window->Print(textSize.str(), 10, 50, RGB(255, 255, 255));
	window->Print(textMode.str(), 10, 75, RGB(255, 255, 255));
	window->Print(textMouse.str(), 10, 100, RGB(255, 255, 255));
}

// ------------------------------------------------------------------------------

void WinApp::Finalize()
{
}


// ------------------------------------------------------------------------------
//                                  WinMain                                      
// ------------------------------------------------------------------------------

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	                 _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	// cria motor e configura a janela
	Engine* engine = new Engine();
	engine->window->Mode(WINDOWED);
	engine->window->Size(1024, 600);
	engine->window->Color(0, 122, 204);
	engine->window->Title("Aplicação Windows");
	engine->window->Icon(IDI_ICON);
	engine->window->Cursor(IDC_CURSOR);

	// cria e executa a aplicação
	int exitCode = engine->Start(new WinApp());

	// finaliza execução
	delete engine;
	return exitCode;
}

// ----------------------------------------------------------------------------

