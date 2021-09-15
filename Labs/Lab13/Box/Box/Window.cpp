/**********************************************************************************
// Window (C�digo Fonte)
// 
// Cria��o:		19 Mai 2007
// Atualiza��o:	17 Abr 2016
// Compilador:	Visual C++ 14
//
// Descri��o:	A classe abstrai todos os detalhes de configura��o de
//              uma janela para um jogo. 
//
**********************************************************************************/

#include "Window.h"
#include "Input.h"

// -------------------------------------------------------------------------------
// Construtor

Window::Window()
{
	windowId	= 0;										// identificador da janela do jogo
	windowWidth		= GetSystemMetrics(SM_CXSCREEN);		// a janela ocupa toda a tela (tela cheia)
	windowHeight	= GetSystemMetrics(SM_CYSCREEN);		// a janela ocupa toda a tela (tela cheia)
	windowIcon		= LoadIcon(NULL, IDI_APPLICATION);		// �cone padr�o de uma aplica��o
	windowCursor	= LoadCursor(NULL, IDC_ARROW);			// cursor padr�o de uma aplica��o
	windowColor	    = RGB(0,0,0);							// cor de fundo padr�o � preta
	windowTitle		= string("Windows Game");				// t�tulo padr�o da janela
	windowStyle		= WS_POPUP | WS_VISIBLE;				// estilo para tela cheia
	windowMode		= FULLSCREEN;							// modo padr�o � tela cheia
	windowPosX		= 0;									// posi��o inicial da janela no eixo x
	windowPosY		= 0;									// posi��o inicial da janela no eixo y
	windowCenterX   = windowWidth/2;						// centro da janela no eixo x
	windowCenterY	= windowHeight/2;						// centro da janela no eixo y
}

// -------------------------------------------------------------------------------

void Window::Mode(int mode)
{
	windowMode = mode;

	if (mode == WINDOWED)
	{
		// modo em janela
		windowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE; 
	}
	else
	{
		// modo em tela cheia ou sem bordas
		windowStyle = WS_EX_TOPMOST | WS_POPUP | WS_VISIBLE; 
	} 
}

// -------------------------------------------------------------------------------

void Window::Size(int width, int height)
{ 
	// window size
	windowWidth = width; 
	windowHeight = height;

	// calcula a posi��o do centro da janela
	windowCenterX = windowWidth/2;
	windowCenterY = windowHeight/2;

	// ajusta a posi��o da janela para o centro da tela
	windowPosX = (GetSystemMetrics(SM_CXSCREEN)/2) - (windowWidth/2);
	windowPosY = (GetSystemMetrics(SM_CYSCREEN)/2) - (windowHeight/2);
}

// -------------------------------------------------------------------------------

void Window::Print(string text, int x, int y, COLORREF color)
{
	// esta fun��o exibe o texto na posi��o (x,y) da tela usando a cor especificada
	// ela usa a GDI do Windows (lenta) e deve ser usada apenas para depura��o

	// pega o contexto do dispositivo gr�fico
	HDC xdc = GetDC(windowId); 

	// define a cor do texto
	SetTextColor(xdc, color);

	// define o fundo do texto como transparente
	SetBkMode(xdc, TRANSPARENT);

	// mostra o texto
	TextOut(xdc, x, y, text.c_str(), (int)text.size());

	// libera o contexto do dispositivo
	ReleaseDC(windowId, xdc);
}

// -------------------------------------------------------------------------------

bool Window::Create()
{
	WNDCLASSEX wndClass; 

	// identificador da aplica��o
	HINSTANCE appId = GetModuleHandle(NULL);
	
	// definindo uma classe de janela chamada "GameWindow"
	wndClass.cbSize         = sizeof(WNDCLASSEX);
	wndClass.style			= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc	= Input::Handler;
	wndClass.cbClsExtra		= 0;
	wndClass.cbWndExtra		= 0;
	wndClass.hInstance		= appId;
	wndClass.hIcon			= windowIcon;
	wndClass.hCursor		= windowCursor; 
	wndClass.hbrBackground	= (HBRUSH) CreateSolidBrush(windowColor);
	wndClass.lpszMenuName	= NULL;
	wndClass.lpszClassName	= "GameWindow";
	wndClass.hIconSm        = windowIcon;

	// registrando "GameWindow"
	if (!RegisterClassEx(&wndClass))
		return false;

	// criando uma janela baseada na classe "GameWindow" 
	windowId = CreateWindowEx(
		NULL,							// estilos extras
		"GameWindow",					// nome da "window class"
		windowTitle.c_str(),			// t�tulo da janela
		windowStyle,					// estilo da janela
		windowPosX, windowPosY,			// posi��o (x,y) inicial
		windowWidth, windowHeight,		// largura e altura da janela
		NULL,							// identificador da janela pai
		NULL,							// identificador do menu
		appId,							// identificador da aplica��o
		NULL);							// par�metros de cria��o

	// Ao usar o modo em janela � preciso levar em conta que as barras 
	// e bordas ocupam espa�o na janela. O c�digo abaixo ajusta o tamanho
	// da janela de forma que a �rea cliente do jogo fique no tamanho 
	// (windowWidth x windowHeight)

	if (windowMode == WINDOWED)
	{
		// ret�ngulo com o tamanho da �rea cliente desejada
		RECT winRect = {0, 0, windowWidth, windowHeight};

		// ajusta o tamanho do ret�ngulo
		AdjustWindowRectEx(&winRect,
			GetWindowStyle(windowId),
			GetMenu(windowId) != NULL,
			GetWindowExStyle(windowId));

		// atualiza posi��o da janela
		windowPosX = (GetSystemMetrics(SM_CXSCREEN)/2) - ((winRect.right - winRect.left)/2);
		windowPosY = (GetSystemMetrics(SM_CYSCREEN)/2) - ((winRect.bottom - winRect.top)/2);

		// redimensiona janela com uma chamada a MoveWindow
		MoveWindow(
			windowId,                  // identificador da janela
			windowPosX,                    // posi��o x
			windowPosY,                    // posi��o y
			winRect.right - winRect.left,  // largura
			winRect.bottom - winRect.top,  // altura
			TRUE);						   // repintar
	}

	// retorna estado da inicializa��o (bem sucedida ou n�o)
	return (windowId ? true : false);
}

// -------------------------------------------------------------------------------

