/**********************************************************************************
// WinDXUT (Código Fonte)
//
// Criação:     04 Jan 2020
// Atualização: 24 Jul 2021
// Compilador:  Visual C++ 2019
//
// Descrição:   Constrói uma janela para testar a classe Window do DXUT
//
**********************************************************************************/

#include "DXUT.h"

// ------------------------------------------------------------------------------
//                                  WinMain                                      
// ------------------------------------------------------------------------------

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	// configura e cria a janela
	Window * window = new Window();
	
	window->Mode(WINDOWED);
	window->Size(1024, 600);
	window->Color(0, 122, 204);
	window->Title("Aplicação");
	window->Icon(IDI_ICON);
	window->Cursor(IDC_CURSOR);
	window->Create();
	
	// trata as mensagens destinadas a janela
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// fim do programa
	delete window;
	return 0;
}

// ----------------------------------------------------------------------------

