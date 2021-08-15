/**********************************************************************************
// Graphics (Arquivo de Cabeçalho)
// 
// Criação:     06 Abr 2011
// Atualização: 15 Ago 2021
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
#include <D3DCompiler.h>         // fornece D3DBlob

enum AllocationType { GPU, UPLOAD };

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

public:
    Graphics();                                             // constructor
    ~Graphics();                                            // destructor

    void VSync(bool state);                                 // liga/desliga vertical sync
    void Initialize(Window * window);                       // inicializa o Direct3D
    void Clear(ID3D12PipelineState * pso);                  // limpa o backbuffer com a cor de fundo
    void Present();                                         // apresenta desenho na tela

    void ResetCommands();                                   // reinicia lista para receber novos comandos
    void SubmitCommands();                                  // submete para execução os comandos pendentes

    void Allocate(uint sizeInBytes,
                  ID3DBlob** resource);                     // aloca memória da CPU para recurso

    void Allocate(uint type,
                  uint sizeInBytes, 
                  ID3D12Resource** resource);               // aloca memória da GPU para recurso

    void Copy(const void* vertices,
              uint sizeInBytes,
              ID3DBlob* bufferCPU);                         // copia vértices para Blob na CPU

    void Copy(const void* vertices, 
              uint sizeInBytes,
              ID3D12Resource* bufferUpload,
              ID3D12Resource* bufferGPU);                   // copia vértices para a GPU

    ID3D12Device4* Device();                                // retorna dispositivo Direct3D
    ID3D12GraphicsCommandList* CommandList();               // retorna lista de comandos
    uint Antialiasing();                                    // retorna número de amostras por pixel
    uint Quality();                                         // retorna qualidade das amostras
};

// --------------------------------------------------------------------------------
// Métodos Inline

// liga/desliga vertical sync
inline void Graphics::VSync(bool state)
{ vSync = state; }

// retorna dispositivo Direct3D
inline ID3D12Device4* Graphics::Device()
{ return device; }

// retorna lista de comandos do dispositivo
inline ID3D12GraphicsCommandList* Graphics::CommandList()
{ return commandList; }

// retorna número de amostras por pixel
inline uint Graphics::Antialiasing()
{ return antialiasing; }

// retorna qualidade das amostras
inline uint Graphics::Quality()
{ return quality; }

// --------------------------------------------------------------------------------

#endif