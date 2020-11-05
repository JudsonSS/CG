/**********************************************************************************
// InputText (Código Fonte)
//
// Criação:		06 Jan 2020
// Atualização:	04 Nov 2020
// Compilador:	Visual C++ 2019
//
// Descrição:	Testa captura de caracteres com a mensagem WM_CHAR
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
	window->Title("Input Text");
	window->Icon(IDI_ICON);
	window->Cursor(IDC_CURSOR);
	window->Create();	

	// leitura da entrada
	Input* input = new Input();

	MSG msg = { 0 };
	
	int x = window->CenterX();
	int y = window->CenterY();

	// trata as mensagens destinadas a janela
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (input->KeyPress(VK_ESCAPE))
			window->Close();

		if (input->KeyPress(VK_INSERT))
			input->Read();

		window->Print(Input::Text(), x - 120, y, RGB(255, 255, 255));
	} 

	// fim do programa
	delete input;
	delete window;
	return 0;
}

// ----------------------------------------------------------------------------
