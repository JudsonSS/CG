/**********************************************************************************
// Graphics (Arquivo de Cabeçalho)
// 
// Criação:     06 Abr 2011
// Atualização: 07 Ago 2021
// Compilador:  Visual C++ 2019
//
// Descrição:   Usa funções do Direct3D 12 para acessar a GPU
//
**********************************************************************************/

#ifndef DXUT_GRAPHICS_H
#define DXUT_GRAPHICS_H

// --------------------------------------------------------------------------------
// Inclusões

#include <dxgi1_6.h>             // infraestrutura gráfica do DirectX
#include <d3d12.h>               // principais funções do Direct3D
#include "Window.h"              // cria e configura uma janela do Windows
#include "Types.h"               // tipos específicos da engine

// --------------------------------------------------------------------------------

class Graphics
{
private:
    // configuração
    uint                         backBufferCount;           // número de buffers na swap chain (double, triple, etc.)
    uint                         antialiasing;              // número de amostras para cada pixel na tela
    uint                         quality;                   // qualidade da amostragem de antialiasing
    bool                         vSync;                     // vertical sync 
    float                        bgColor[4];                // cor de fundo do backbuffer

    // pipeline
    ID3D12Device4              * device;                    // dispositivo gráfico
    IDXGIFactory6              * factory;                   // interface principal da DXGI
    IDXGISwapChain1            * swapChain;                 // swap chain
    uint                         backBufferIndex;           // índice do backbuffer atual
    
    ID3D12CommandQueue         * commandQueue;              // fila de comandos da GPU
    ID3D12GraphicsCommandList  * commandList;               // lista de comandos a submeter para GPU
    ID3D12CommandAllocator     * commandListAlloc;          // memória utilizada pela lista de comandos
     
    ID3D12Resource            ** renderTargets;             // buffers para renderização (front e back)
    ID3D12Resource             * depthStencil;              // buffer de profundidade e estampa            
    ID3D12DescriptorHeap       * renderTargetHeap;          // descriptor heap para os Render Targets
    ID3D12DescriptorHeap       * depthStencilHeap;          // descriptor heap para o Depth Stencil
    uint                         rtDescriptorSize;          // tamanho de cada descritor de Render Target
    D3D12_VIEWPORT               viewport;                  // viewport
    D3D12_RECT                   scissorRect;               // retângulo de corte

    // sincronização                         
    ID3D12Fence                * fence;                     // barreira para sincronizar CPU/GPU
    ullong                       currentFence;              // contador de barreiras

    // métodos privados
    void LogHardwareInfo();                                 // mostra informações do hardware
    bool WaitCommandQueue();                                // espera execução da fila de comandos
    void SubmitCommands();                                  // submete para execução os comandos pendentes

public:
    Graphics();                                             // constructor
    ~Graphics();                                            // destructor

    void VSync(bool state);                                 // liga/desliga vertical sync
    void Initialize(Window * window);                       // inicializa o Direct3D
    void Clear(ID3D12PipelineState * pso);                  // limpa o backbuffer com a cor de fundo
    void Present();                                         // apresenta desenho na tela
};

// --------------------------------------------------------------------------------
// Métodos Inline

// liga/desliga vertical sync
inline void Graphics::VSync(bool state)
{ vSync = state; }

// --------------------------------------------------------------------------------

#endif