/**********************************************************************************
// WinDXUT (C�digo Fonte)
//
// Cria��o:     04 Jan 2020
// Atualiza��o: 24 Jul 2021
// Compilador:  Visual C++ 2019
//
// Descri��o:   Constr�i uma janela para testar a classe Window do DXUT
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
	window->Title("Aplica��o");
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

