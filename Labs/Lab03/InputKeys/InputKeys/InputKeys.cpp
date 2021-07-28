/**********************************************************************************
// InputKeys (Código Fonte)
//
// Criação:		06 Jan 2020
// Atualização:	27 Jul 2021
// Compilador:	Visual C++ 2019
//
// Descrição:	Testa funcionalidades de leitura do teclado
//
**********************************************************************************/

#include "DXUT.h"

// ------------------------------------------------------------------------------
//                                  WinMain                                      
// ------------------------------------------------------------------------------

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	// configuração e criação da janela
	Window* window = new Window();
	window->Mode(WINDOWED);
	window->Size(1024, 600);
	window->Color(0, 122, 204);
	window->Title("Input Keys");
	window->Icon(IDI_ICON);
	window->Cursor(IDC_CURSOR);
	window->Create();

	// leitura da entrada
	Input* input = new Input();

	MSG msg = { 0 };
	
	int x = window->CenterX();
	int y = window->CenterY();

	window->Print("Teste de Captura de Teclas", x - 110, y - 150, RGB(255,255,255));

	// trata as mensagens destinadas a janela
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (input->KeyPress(VK_ESCAPE))
			window->Close();

		// CTRL, ENTER, ESPAÇO
		if (input->KeyDown(VK_CONTROL))
			window->Print("CTRL", x-125, y+150, RGB(255,255,255));
		if (input->KeyDown(VK_RETURN))
			window->Print("ENTER", x-25, y+150, RGB(255,255,255));
		if (input->KeyDown(VK_SPACE))
			window->Print("SPACE", x+75, y+150, RGB(255,255,255));

		// setas de direção
		if (input->KeyDown(VK_LEFT))
			window->Print("LEFT", x-45, y, RGB(255,255,255));
		if (input->KeyDown(VK_RIGHT))
			window->Print("RIGHT", x, y, RGB(255,255,255));
		if (input->KeyDown(VK_UP))
			window->Print("UP", x-10, y-20, RGB(255,255,255));
		if (input->KeyDown(VK_DOWN))
			window->Print("DOWN", x-25, y+20, RGB(255,255,255));

		// teclar R
		if (input->KeyPress('R'))
		{
			window->Clear();
			window->Print("Teste de Captura de Teclas", x - 110, y - 150, RGB(255,255,255));
		}
	} 

	// fim do programa
	delete input;
	delete window;
	return 0;
}

// ----------------------------------------------------------------------------