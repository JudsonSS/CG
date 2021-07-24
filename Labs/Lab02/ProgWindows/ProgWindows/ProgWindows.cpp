/**********************************************************************************
// ProgWindows
// 
// Cria��o:		04 Jan 2020
// Atualiza��o:	24 Jul 2021
// Compilador:	Visual C++ 2019
//
// Descri��o:	Esqueleto b�sico de cria��o de uma janela no Windows 
//              usando a API Win32. Este esqueleto pode ser usado como ponto
//              de partida para cria��o de qualquer aplica��o.
//
**********************************************************************************/

// inclui tipos b�sicos e fun��es da API Win32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// prot�tipo do procedimento da janela
LRESULT CALLBACK WinProc (HWND, UINT, WPARAM, LPARAM);

//--------------------------------------------------------------------------------

// programa principal
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
     HWND         hwnd;        // identificador da janela
     MSG          msg;         // mensagem
     WNDCLASS     wndclass;    // classe da janela

	 // Definindo uma classe de janela chamada "AppWindow" 
     wndclass.style         = CS_HREDRAW | CS_VREDRAW;
     wndclass.lpfnWndProc   = WinProc;
     wndclass.cbClsExtra    = 0;
     wndclass.cbWndExtra    = 0;
     wndclass.hInstance     = hInstance;
     wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
     wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
     wndclass.lpszMenuName  = NULL;
     wndclass.lpszClassName = "AppWindow";

	 // Registrando a classe "AppWindow"
     if (!RegisterClass (&wndclass))
     {
          MessageBox (NULL, "Erro na cria��o da janela!", "Aplica��o", MB_ICONERROR);
          return 0 ;
     }

	 // Criando uma janela baseada na classe "AppWindow"
     hwnd = CreateWindow("AppWindow",			            // classe da janela
                          "Aplica��o",	                    // t�tulo da janela
                          WS_OVERLAPPEDWINDOW | 
                          WS_VISIBLE,                       // estilo da janela
                          CW_USEDEFAULT,                    // posi��o x inicial
                          CW_USEDEFAULT,                    // posi��o y inicial
                          CW_USEDEFAULT,                    // largura inicial
                          CW_USEDEFAULT,                    // altura inicial
                          NULL,                             // identificador da janela pai
                          NULL,                             // identificador do menu
                          hInstance,                        // identificador da aplica��o
                          NULL);                            // par�metros de cria��o
     
	 // Tratamento de mensagens destinadas a janela da aplica��o
     while (GetMessage(&msg, NULL, 0, 0))
     {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
     }

	 // fim do programa
     return 0;
}

//-------------------------------------------------------------------------------

// procedimento da janela
LRESULT CALLBACK WinProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{     
     switch (message)
     {
     // janela foi fechada
     case WM_DESTROY:
		 PostQuitMessage(0);
         return 0;
     }
     return DefWindowProc(hwnd, message, wParam, lParam);
}

//-------------------------------------------------------------------------------