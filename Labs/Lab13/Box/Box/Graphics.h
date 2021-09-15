/**********************************************************************************
// Graphics (Arquivo de Cabeçalho)
// 
// Criação:		06 Abr 2011
// Atualização:	27 Abr 2016
// Compilador:	Visual C++ 14
//
// Descrição:	Graphics é uma classe que faz uso das funções do Direct 3D 12 
//              para acessar a unidade de processamento gráfico (GPU).
//
**********************************************************************************/

#ifndef _DX12_GRAPHICS_H_
#define _DX12_GRAPHICS_H_

// --------------------------------------------------------------------------------
// Inclusões

#include <dxgi1_4.h>		// infraestrutura gráfica do DirectX
#include <d3d12.h>			// principais funções do Direct3D
#include "Window.h"			// cria e configura uma janela do Windows
#include "Types.h"			// tipos específicos da engine

// --------------------------------------------------------------------------------

class Graphics
{
private:
	// configuração
	uint						 backBufferCount;			// número de buffers na swap chain (double, triple, etc.)
	uint						 antialiasing;				// número de amostras para cada pixel na tela
	uint                         quality;					// qualidade da amostragem de antialiasing
	bool						 vSync;						// vertical sync 
	float						 bgColor[4];				// cor de fundo do backbuffer

	// pipeline
	ID3D12Device               * device;					// dispositivo gráfico
	IDXGIFactory4              * factory;					// interface principal da DXGI
	IDXGISwapChain             * swapChain;					// swap chain
	uint                         backBufferIndex;			// índice do backbuffer atual
	
	ID3D12CommandQueue         * commandQueue;				// fila de comandos da GPU
	ID3D12GraphicsCommandList  * commandList;				// lista de comandos a submeter para GPU
	ID3D12CommandAllocator     * commandListAlloc;			// memória utilizada pela lista de comandos
	 
	ID3D12Resource            ** renderTargets;				// buffers para rendererização (front e back buffer)
	ID3D12Resource             * depthStencil;				// buffer de profundidade e estampa			
	ID3D12DescriptorHeap       * renderTargetHeap;			// descriptor heap para os Render Targets
	ID3D12DescriptorHeap       * depthStencilHeap;			// descriptor heap para o Depth Stencil
	uint                         rtIncrementSize;			// tamanho de cada descritor de Render Target
	uint                         dsIncrementSize;			// tamanho de cada descritor de Depth Stencil
	D3D12_VIEWPORT		         viewport;					// viewport
	D3D12_RECT                   scissorRect;				// retângulo de corte

	// sincronização                         
	ID3D12Fence                * fence;						// barreira para sincronizar CPU/GPU
	ullong                       currentFence;				// contador de barreiras

	void LogHardwareInfo();									// mostra informações do hardware
	bool WaitCommandQueue();								// espera execução da fila de comandos (ineficiente)

public:
	Graphics();												// constructor
	~Graphics();											// destructor

	bool Initialize(Window * window);						// inicializa o Direct3D
	void VSync(bool state);									// liga/desliga vertical sync
	void Clear(ID3D12PipelineState * pso);					// limpa o backbuffer com a cor de fundo
	void Present();											// apresenta desenho na tela
	void ResetCommands();									// reinicia lista para receber novos comandos
	void SubmitCommands();									// submete para execução os comandos pendentes

	ID3D12GraphicsCommandList * CommandList();				// retorna apontador para a lista de comandos
	ID3D12Device * Device();								// retorna dispositivo Direct3D
	D3D12_VIEWPORT Viewport();								// retorna a viewport utilizada	
	uint Antialiasing();									// retorna número de amostras por pixel
	uint Quality();											// retorna qualidade das amostras
	
};

// --------------------------------------------------------------------------------
// Métodos Inline

// liga/desliga vertical sync
inline void Graphics::VSync(bool state)
{ vSync = state; }

// retorna lista de comandos do dispositivo
inline ID3D12GraphicsCommandList * Graphics::CommandList()
{ return commandList; }

// retorna dispositivo Direct3D
inline ID3D12Device * Graphics::Device()
{ return device; }

// retorna a viewport utilizada
inline D3D12_VIEWPORT Graphics::Viewport()
{ return viewport; }
									
// retorna número de amostras por pixel
inline uint Graphics::Antialiasing()
{ return antialiasing; }

// retorna qualidade das amostras
inline uint Graphics::Quality()
{ return quality; }

// --------------------------------------------------------------------------------

#endif