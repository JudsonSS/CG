/**********************************************************************************
// InputFocus (C�digo Fonte)
//
// Cria��o:		06 Jan 2020
// Atualiza��o:	04 Nov 2020
// Compilador:	Visual C++ 2019
//
// Descri��o:	Testa entrada de dados com a mudan�a de foco da janela
//
**********************************************************************************/

#include "DXUT.h"

// Prot�tipo de fun��es e declara��es globais
Window* window;
void Welcome();
void Bye();

// ------------------------------------------------------------------------------
//                                  WinMain                                      
// ------------------------------------------------------------------------------

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	// configura��o e cria��o da janela
	window = new Window();
	window->Mode(WINDOWED);
	window->Size(1024, 600);
	window->Color(0, 122, 204);
	window->Title("Input Focus");
	window->Icon(IDI_ICON);
	window->Cursor(IDC_CURSOR);
	//-------- FOCUS ----------
	window->InFocus(Welcome);
	window->LostFocus(Bye);
	//-------------------------
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
	} 

	// fim do programa
	delete input;
	delete window;
	return 0;
}

// ----------------------------------------------------------------------------

void Welcome()
{
	window->Clear();
	window->Print("De volta?", 485, 300, RGB(255, 255, 255));
}

void Bye()
{
	window->Clear();
	window->Print("At� Logo!", 485, 300, RGB(255, 255, 255));
}

// ----------------------------------------------------------------------------


