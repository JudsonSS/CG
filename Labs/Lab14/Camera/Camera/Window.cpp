/**********************************************************************************
// Window (Código Fonte)
// 
// Criação:     19 Mai 2007
// Atualização: 09 Ago 2021
// Compilador:  Visual C++ 2019
//
// Descrição:   Abstrai os detalhes de configuração de uma janela
//
**********************************************************************************/

#include "Window.h"

// -------------------------------------------------------------------------------
// inicialização de membros estáticos da classe

void (*Window::inFocus)() = nullptr;                        // nenhuma ação ao ganhar foco
void (*Window::lostFocus)() = nullptr;                      // nenhuma ação ao perder foco

// -------------------------------------------------------------------------------
// construtor

Window::Window()
{
    windowId      = 0;                                      // id nulo porque a janela ainda não existe
    windowWidth   = GetSystemMetrics(SM_CXSCREEN);          // a janela ocupa toda a tela (tela cheia)
    windowHeight  = GetSystemMetrics(SM_CYSCREEN);          // a janela ocupa toda a tela (tela cheia)
    windowIcon    = LoadIcon(NULL, IDI_APPLICATION);        // ícone padrão de uma aplicação
    windowCursor  = LoadCursor(NULL, IDC_ARROW);            // cursor padrão de uma aplicação
    windowColor   = RGB(0,0,0);                             // cor de fundo padrão é preta
    windowTitle   = string("Windows App");                  // título padrão da janela
    windowStyle   = WS_POPUP | WS_VISIBLE;                  // estilo para tela cheia
    windowMode    = FULLSCREEN;                             // modo padrão é tela cheia
    windowPosX    = 0;                                      // posição inicial da janela no eixo x
    windowPosY    = 0;                                      // posição inicial da janela no eixo y
    windowCenterX = windowWidth/2;                          // centro da janela no eixo x
    windowCenterY = windowHeight/2;                         // centro da janela no eixo y
    windowHdc     = { 0 };                                  // contexto do dispositivo
    windowRect    = { 0, 0, 0, 0 };                         // área cliente da janela
}

// -------------------------------------------------------------------------------

Window::~Window()
{
    // libera contexto do dispositivo
    if (windowHdc) ReleaseDC(windowId, windowHdc);
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
        // modo em tela cheia / janela sem bordas
        windowStyle = WS_EX_TOPMOST | WS_POPUP | WS_VISIBLE; 
    } 
}

// -------------------------------------------------------------------------------

void Window::Size(int width, int height)
{ 
    // window size
    windowWidth = width; 
    windowHeight = height;

    // calcula a posição do centro da janela
    windowCenterX = windowWidth/2;
    windowCenterY = windowHeight/2;

    // ajusta a posição da janela para o centro da tela
    windowPosX = (GetSystemMetrics(SM_CXSCREEN)/2) - (windowWidth/2);
    windowPosY = (GetSystemMetrics(SM_CYSCREEN)/2) - (windowHeight/2);
}

// -------------------------------------------------------------------------------

bool Window::Create()
{
    // identificador da aplicação
    HINSTANCE appId = GetModuleHandle(NULL);
    
    // definindo uma classe de janela
    WNDCLASSEX wndClass;     
    wndClass.cbSize        = sizeof(WNDCLASSEX);
    wndClass.style         = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc   = Window::WinProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = 0;
    wndClass.hInstance     = appId;
    wndClass.hIcon         = windowIcon;
    wndClass.hCursor       = windowCursor; 
    wndClass.hbrBackground = (HBRUSH) CreateSolidBrush(windowColor);
    wndClass.lpszMenuName  = NULL;
    wndClass.lpszClassName = "AppWindow";
    wndClass.hIconSm       = windowIcon;

    // registrando classe "AppWindow"
    if (!RegisterClassEx(&wndClass))
        return false;

    // criando uma janela baseada na classe "AppWindow" 
    windowId = CreateWindowEx(
        NULL,                              // estilos extras
        "AppWindow",                       // nome da classe da janela
        windowTitle.c_str(),               // título da janela
        windowStyle,                       // estilo da janela
        windowPosX, windowPosY,            // posição (x,y) inicial
        windowWidth, windowHeight,         // largura e altura da janela
        NULL,                              // identificador da janela pai
        NULL,                              // identificador do menu
        appId,                             // identificador da aplicação
        NULL);                             // parâmetros de criação

    // Ao usar o modo em janela é preciso levar em conta que as barras 
    // e bordas ocupam espaço na janela. O código abaixo ajusta o tamanho
    // da janela de forma que a área cliente fique com tamanho 
    // (windowWidth x windowHeight)

    if (windowMode == WINDOWED)
    {
        // retângulo com o tamanho da área cliente desejada
        RECT rect = {0, 0, windowWidth, windowHeight};

        // ajusta o tamanho do retângulo
        AdjustWindowRectEx(&rect,
            GetWindowStyle(windowId),
            GetMenu(windowId) != NULL,
            GetWindowExStyle(windowId));

        // atualiza posição da janela
        windowPosX = (GetSystemMetrics(SM_CXSCREEN)/2) - ((rect.right - rect.left)/2);
        windowPosY = (GetSystemMetrics(SM_CYSCREEN)/2) - ((rect.bottom - rect.top)/2);

        // redimensiona janela com uma chamada a MoveWindow
        MoveWindow(
            windowId,                      // identificador da janela
            windowPosX,                    // posição x
            windowPosY,                    // posição y
            rect.right - rect.left,        // largura
            rect.bottom - rect.top,        // altura
            TRUE);                         // repintar
    }

    // captura contexto do dispositivo
    windowHdc = GetDC(windowId);

    // pega tamanho da área cliente
    GetClientRect(windowId, &windowRect);

    // retorna estado da inicialização (bem sucedida ou não)
    return (windowId ? true : false);
}

// -------------------------------------------------------------------------------

LRESULT CALLBACK Window::WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    // janela perdeu o foco
    case WM_KILLFOCUS:
        if (lostFocus)
            lostFocus();
        return 0;

    // janela recebeu o foco
    case WM_SETFOCUS:
        if (inFocus)
            inFocus();
        return 0;

    // a janela foi destruida
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// -----------------------------------------------------------------------------
