/**********************************************************************************
// Graphics (Arquivo de Cabe�alho)
// 
// Cria��o:     06 Abr 2011
// Atualiza��o: 10 Ago 2022
// Compilador:  Visual C++ 2012
//
// Descri��o:   Usa fun��es do Direct3D 12 para acessar a GPU
//
**********************************************************************************/

#ifndef DXUT_GRAPHICS_H
#define DXUT_GRAPHICS_H

// --------------------------------------------------------------------------------
// Inclus�es

#include <dxgi1_6.h>    // infraestrutura gr�fica do DirectX
#include <d3d12.h>      // principais fun��es do Direct3D
#include "Window.h"     // cria e configura uma janela do Windows
#include "Types.h"      // tipos espec�ficos da engine

// --------------------------------------------------------------------------------

class Graphics
{
private:
    // pipeline
    ID3D12Device4              * device;                    // dispositivo gr�fico
    IDXGIFactory6              * factory;                   // interface principal da DXGI
    
    ID3D12CommandQueue         * commandQueue;              // fila de comandos da GPU
    ID3D12GraphicsCommandList  * commandList;               // lista de comandos a submeter para GPU
    ID3D12CommandAllocator     * commandListAlloc;          // mem�ria utilizada pela lista de comandos

    // sincroniza��o                         
    ID3D12Fence                * fence;						// barreira para sincronizar CPU/GPU
    ullong                       currentFence;				// contador de barreiras

    void LogHardwareInfo();									// mostra informa��es do hardware
    bool WaitCommandQueue();								// espera execu��o da fila de comandos
    void SubmitCommands();									// submete para execu��o os comandos pendentes

public:
    Graphics();												// constructor
    ~Graphics();											// destructor

    void Initialize(Window * window);						// inicializa o Direct3D
};

// --------------------------------------------------------------------------------

#endif