/**********************************************************************************
// Graphics (Arquivo de Cabeçalho)
// 
// Criação:     06 Abr 2011
// Atualização: 10 Ago 2022
// Compilador:  Visual C++ 2012
//
// Descrição:   Usa funções do Direct3D 12 para acessar a GPU
//
**********************************************************************************/

#ifndef DXUT_GRAPHICS_H
#define DXUT_GRAPHICS_H

// --------------------------------------------------------------------------------
// Inclusões

#include <dxgi1_6.h>    // infraestrutura gráfica do DirectX
#include <d3d12.h>      // principais funções do Direct3D
#include "Window.h"     // cria e configura uma janela do Windows
#include "Types.h"      // tipos específicos da engine

// --------------------------------------------------------------------------------

class Graphics
{
private:
    // pipeline
    ID3D12Device4              * device;                    // dispositivo gráfico
    IDXGIFactory6              * factory;                   // interface principal da DXGI
    
    ID3D12CommandQueue         * commandQueue;              // fila de comandos da GPU
    ID3D12GraphicsCommandList  * commandList;               // lista de comandos a submeter para GPU
    ID3D12CommandAllocator     * commandListAlloc;          // memória utilizada pela lista de comandos

    // sincronização                         
    ID3D12Fence                * fence;						// barreira para sincronizar CPU/GPU
    ullong                       currentFence;				// contador de barreiras

    void LogHardwareInfo();									// mostra informações do hardware
    bool WaitCommandQueue();								// espera execução da fila de comandos
    void SubmitCommands();									// submete para execução os comandos pendentes

public:
    Graphics();												// constructor
    ~Graphics();											// destructor

    void Initialize(Window * window);						// inicializa o Direct3D
};

// --------------------------------------------------------------------------------

#endif