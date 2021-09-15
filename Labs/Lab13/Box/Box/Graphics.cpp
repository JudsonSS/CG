/**********************************************************************************
// Graphics (Código Fonte)
// 
// Criação:		06 Abr 2011
// Atualização:	27 Abr 2016
// Compilador:	Visual C++ 14
//
// Descrição:	Graphics é uma classe que faz uso das funções do Direct 3D 12
//              para acessar a unidade de processamento gráfico (GPU).
//
**********************************************************************************/

#include "Graphics.h"
#include <sstream>
using namespace std;

// ------------------------------------------------------------------------------

Graphics::Graphics()
{
	
	backBufferCount = 2;	// front e back buffer
	antialiasing = 1;		// sem antialiasing
	quality = 0;			// qualidade padrão
	vSync = false;			// sem vertical sync

	bgColor[0] = 0.0f;		// componente Red
	bgColor[1] = 0.0f;		// componente Green
	bgColor[2] = 0.0f;		// componente Blue
	bgColor[3] = 0.0f;		// componente Alpha (sólido)


	factory           = nullptr;
	device            = nullptr;
	swapChain         = nullptr;
	backBufferIndex   = 0;
	commandQueue      = nullptr;
	commandList       = nullptr;
	commandListAlloc  = nullptr;
	
	renderTargets     = new ID3D12Resource*[backBufferCount] {nullptr};
	depthStencil      = nullptr;	
	renderTargetHeap  = nullptr;
	depthStencilHeap  = nullptr;
	rtIncrementSize   = 0;
	dsIncrementSize   = 0;

	ZeroMemory(&viewport, sizeof(viewport));
	ZeroMemory(&scissorRect, sizeof(scissorRect));

	fence = nullptr;
	currentFence = 0;
}

// ------------------------------------------------------------------------------

Graphics::~Graphics()
{
	// libera depth stencil buffer
	if (depthStencil)
		depthStencil->Release();

	// libera render targets buffers
	if (renderTargets)
	{
		for (uint i = 0; i < backBufferCount; ++i)
		{
			if (renderTargets[i])
				renderTargets[i]->Release();
		}
		delete[] renderTargets;
	}

	// libera barreira
	if (fence)
		fence->Release();

	// libera depth stencil heap
	if (depthStencilHeap)
		depthStencilHeap->Release();

	// libera render target heap
	if (renderTargetHeap)
		renderTargetHeap->Release();

	// libera swap chain
	if (swapChain)
	{
		// Direct3D é incapaz de fechar quando em tela cheia
		swapChain->SetFullscreenState(false, NULL);
		swapChain->Release();
	}

	// libera lista de comandos
	if (commandList)
		commandList->Release();

	// libera alocador de comandos
	if (commandListAlloc)
		commandListAlloc->Release();

	// libera fila de comandos
	if (commandQueue)
		commandQueue->Release();

	// libera dispositivo gráfico
	if (device)
		device->Release();

	// libera interface principal
	if (factory)
		factory->Release();
}

// ------------------------------------------------------------------------------

void Graphics::LogHardwareInfo()
{
	// --------------------------------------
	// Adaptador de vídeo (placa de vídeo)
	// --------------------------------------
	IDXGIAdapter * adapter = nullptr;
	if (factory->EnumAdapters(0, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		wstringstream text;
		text << L"---> Placa de vídeo: " << desc.Description << L"\n";
		OutputDebugStringW(text.str().c_str());
	}

	// -----------------------------------------
	// Feature Level máximo suportado pela GPU
	// -----------------------------------------
	D3D_FEATURE_LEVEL featureLevels[9] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevelsInfo;
	featureLevelsInfo.NumFeatureLevels = 9;
	featureLevelsInfo.pFeatureLevelsRequested = featureLevels;
	
	device->CheckFeatureSupport(
		D3D12_FEATURE_FEATURE_LEVELS,
		&featureLevelsInfo,
		sizeof(featureLevelsInfo));

	// bloco de instruções
	{
		wstringstream text;
		text << L"---> Feature Level: ";
		switch (featureLevelsInfo.MaxSupportedFeatureLevel)
		{
		case D3D_FEATURE_LEVEL_12_1: text << L"12_1\n"; break;
		case D3D_FEATURE_LEVEL_12_0: text << L"12_0\n"; break;
		case D3D_FEATURE_LEVEL_11_1: text << L"11_1\n"; break;
		case D3D_FEATURE_LEVEL_11_0: text << L"11_0\n"; break;
		case D3D_FEATURE_LEVEL_10_1: text << L"10_1\n"; break;
		case D3D_FEATURE_LEVEL_10_0: text << L"10_0\n"; break;
		case D3D_FEATURE_LEVEL_9_3:  text << L"9_3\n";  break;
		case D3D_FEATURE_LEVEL_9_2:  text << L"9_2\n";  break;
		case D3D_FEATURE_LEVEL_9_1:  text << L"9_1\n";  break;
		}
		OutputDebugStringW(text.str().c_str());
	}


	// -----------------------------------------
	// Saída de vídeo (monitor)
	// -----------------------------------------

	IDXGIOutput* output = nullptr;
	if (adapter->EnumOutputs(0, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		wstringstream text;
		text << L"---> Monitor: " << desc.DeviceName << L"\n";
		OutputDebugStringW(text.str().c_str());
	}

	// ------------------------------------------
	// Modo de vídeo (resolução)
	// ------------------------------------------

	uint numModes = 0;

	// pega quantidade de modos de vídeo suportados
	output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numModes, nullptr);

	// armazena modos de vídeo em um vetor
	DXGI_MODE_DESC * modeList = new DXGI_MODE_DESC[numModes];
	output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numModes, modeList);

	// pega as dimensões da tela
	uint screenWidth = GetSystemMetrics(SM_CXSCREEN);
	uint screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// numerador e denominador da taxa de atualização do monitor
	uint num = 60;
	uint den = 1;

	// atualiza numerador e denominador com os valores de um modo de vídeo compatível com as dimensões da tela
	for (uint i = 0; i < numModes; ++i)
	{	
		if (modeList[i].Width == screenWidth && modeList[i].Height == screenHeight)
		{
			num = modeList[i].RefreshRate.Numerator;
			den = modeList[i].RefreshRate.Denominator;
		}
	}

	wstringstream text;
	text << L"---> Resolução: " << screenWidth << L"x" << screenHeight << L" " << num / den << L" Hz\n";
	OutputDebugStringW(text.str().c_str());

	// libera memória e interfaces DXGI utilizadas
	delete[] modeList;
	if (adapter) adapter->Release();
	if (output)  output->Release();
}

// ------------------------------------------------------------------------------


bool Graphics::WaitCommandQueue()
{
	// avança o valor da barreira para marcar novos comandos a partir desse ponto
	currentFence++;

	// adiciona uma instrução na fila de comandos para inserir uma nova barreira
	// GPU vai finalizar todos os comandos em curso antes de processar esse sinal
	if FAILED(commandQueue->Signal(fence, currentFence))
		return false;

	// espera a GPU completar todos os comandos anteriores
	if (fence->GetCompletedValue() < currentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		// aciona evento quando a GPU atingir a barreira atual  
		if FAILED(fence->SetEventOnCompletion(currentFence, eventHandle))
			return false;

		// espera até o evento ser acionado
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	return true;
}

// -----------------------------------------------------------------------------

bool Graphics::Initialize(Window * window)
{
	// habilita a camada de depuração do D3D12
#ifdef _DEBUG
	ID3D12Debug * debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		debugController->EnableDebugLayer();
#endif

	// cria objeto para infraestrutura gráfica do DirectX (DXGI)
	if FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory)))
		return false;

	// cria objeto para dispositivo gráfico
	if FAILED(D3D12CreateDevice(
		nullptr,								// adaptador de vídeo (nullptr = adaptador padrão)
		D3D_FEATURE_LEVEL_11_0,					// versão mínima dos recursos do Direct3D
		IID_PPV_ARGS(&device)))					// guarda o dispositivo D3D criado				
		return false;

	// exibe informações do hardware gráfico no Output do Visual Studio
#ifdef _DEBUG
	LogHardwareInfo();
#endif 

	// cria barreira para sincronizar CPU/GPU
	if FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)))
		return false;

	// ---------------------------------------------------
	// Cria fila, lista e alocador de commandos
	// ---------------------------------------------------

	// descreve e cria a fila de comandos para a GPU
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	if FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)))
		return false;

	// cria o alocador de comandos
	if FAILED(device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandListAlloc)))
		return false;

	// cria a lista de comandos
	if FAILED(device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandListAlloc,					// alocador de comandos associado
		nullptr,							// estado inicial do pipeline
		IID_PPV_ARGS(&commandList)))
		return false;

	// inicia a lista no estado fechada 
	// antes do primeiro uso ela será reiniciada e precisa estar fechada para isso
	commandList->Close();

	// ------------------------------------------
	// Cria a swap chain
	// ------------------------------------------

	// descreve e cria a swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Width = window->Width();
	swapChainDesc.BufferDesc.Height = window->Height();
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = antialiasing;
	swapChainDesc.SampleDesc.Quality = quality;
	swapChainDesc.BufferCount = backBufferCount;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = window->Id();
	swapChainDesc.Windowed = (window->Mode() == WINDOWED);
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	
	if FAILED(factory->CreateSwapChain(
		commandQueue,
		&swapChainDesc,
		&swapChain))
		return false;
	
	// ------------------------------------------------------------------------
	// Cria heaps para descritores de recursos (render target e depth/stencil)
	// ------------------------------------------------------------------------
	
	// descreve e cria uma heap para o descritor da render target (RT)
	D3D12_DESCRIPTOR_HEAP_DESC renderTargetHeapDesc = {};
	renderTargetHeapDesc.NumDescriptors = backBufferCount;
	renderTargetHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	renderTargetHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if FAILED(device->CreateDescriptorHeap(&renderTargetHeapDesc, IID_PPV_ARGS(&renderTargetHeap)))
		return false;

	// descreve e cria uma heap para o descritor do buffer depth/stencil (DS)
	D3D12_DESCRIPTOR_HEAP_DESC depthstencilHeapDesc = {};
	depthstencilHeapDesc.NumDescriptors = 1;
	depthstencilHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	depthstencilHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if FAILED(device->CreateDescriptorHeap(&depthstencilHeapDesc, IID_PPV_ARGS(&depthStencilHeap)))
		return false;

	// valor a incrementar para acessar o próximo descritor dentro das heaps
	// o tamanho de um descritor depende do hardware gráfico e do tipo de heap utilizada
	rtIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	dsIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	// ----------------------------------------
	// Sincroniza CPU/GPU
	// ----------------------------------------
	
	WaitCommandQueue();

	// limpa lista para receber novos comandos
	if FAILED(commandList->Reset(commandListAlloc, nullptr))
		return false;

	// ------------------------------------------------------
	// Cria recursos - Render Target e Depth/Stencil Buffers
	// ------------------------------------------------------
	
	D3D12_CPU_DESCRIPTOR_HANDLE rtHandle = renderTargetHeap->GetCPUDescriptorHandleForHeapStart();

	// cria um descritor (view) de Render Target para cada buffer (front e back buffer)
	for (uint i = 0; i < backBufferCount; ++i)
	{
		swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
		device->CreateRenderTargetView(renderTargets[i], nullptr, rtHandle);
		rtHandle.ptr += rtIncrementSize;
	}

	// descrição do buffer Depth/Stencil
	D3D12_RESOURCE_DESC depthStencilDesc = {};
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = window->Width();
	depthStencilDesc.Height = window->Height();
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = antialiasing;
	depthStencilDesc.SampleDesc.Quality = quality;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// propriedades da Heap do buffer Depth/Stencil
	D3D12_HEAP_PROPERTIES dsHeapProperties = {};
	dsHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	dsHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	dsHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	dsHeapProperties.CreationNodeMask = 1;
	dsHeapProperties.VisibleNodeMask = 1;

	// descreve valores para limpeza do Depth/Stencil buffer
	D3D12_CLEAR_VALUE optmizedClear = {};
	optmizedClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optmizedClear.DepthStencil.Depth = 1.0f;
	optmizedClear.DepthStencil.Stencil = 0;

	// cria um buffer Depth/Stencil (e sua heap associada)
	if FAILED(device->CreateCommittedResource(
		&dsHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optmizedClear,
		IID_PPV_ARGS(&depthStencil)))
		return false;

	D3D12_CPU_DESCRIPTOR_HANDLE dsHandle = depthStencilHeap->GetCPUDescriptorHandleForHeapStart();

	// cria um descritor (view) de Depth/Stencil para o mip nível 0
	device->CreateDepthStencilView(depthStencil, nullptr, dsHandle);

	// faz a transição do estado inicial do recurso para ser usado como buffer de profundidade
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = depthStencil;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);

	// executa o comando de transição do depth/stencil buffer
	commandList->Close();
	ID3D12CommandList* cmdsLists[] = { commandList };
	commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// espera execução dos comandos
	WaitCommandQueue();

	// ajusta a viewport
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(window->Width());
	viewport.Height = static_cast<float>(window->Height());
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// ajusta o retângulo de corte
	scissorRect = { 0, 0, window->Width(), window->Height() };

	// ajusta a cor de fundo do backbuffer
	// para a mesma cor de fundo da janela
	COLORREF color = window->Color();

	bgColor[0] = GetRValue(color)/255.0f;						// componente Red
	bgColor[1] = GetGValue(color)/255.0f;						// componente Green
	bgColor[2] = GetBValue(color)/255.0f;						// componente Blue
	bgColor[3] = 1.0f;											// componente Alpha (sólido)

	// inicialização bem sucedida
	return true;
}

// -----------------------------------------------------------------------------

void Graphics::Clear(ID3D12PipelineState * pso)
{
	// reutiliza a memória associada com a lista de comandos
	// a reinicialização só pode acontecer quando a lista de comandos tiver terminado de executar na GPU
	commandListAlloc->Reset();

	// uma lista de comandos pode ser reinicializada depois de adicionada à fila de comandos da GPU (via ExecuteCommandList)
	// reutilizando a lista de comandos reutiliza memória
	commandList->Reset(commandListAlloc, pso);

	// indica que o backbuffer será usado como alvo de renderização
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = renderTargets[backBufferIndex];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);

	// ajusta a viewport e retângulos de corte
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);

	// limpa o backbuffer e depth/stencil buffer	
	D3D12_CPU_DESCRIPTOR_HANDLE dsHandle = depthStencilHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE rtHandle = renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
	rtHandle.ptr += backBufferIndex * rtIncrementSize;
	commandList->ClearRenderTargetView(rtHandle, bgColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// especifica quais buffers serão utilizados na renderização
	commandList->OMSetRenderTargets(1, &rtHandle, true, &dsHandle);
}

// -----------------------------------------------------------------------------

void Graphics::Present()
{
	// indica que o backbuffer será usado para apresentação
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = renderTargets[backBufferIndex];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);

	// comandos finalizados
	commandList->Close();

	// adiciona lista de comandos à fila de execução
	ID3D12CommandList * cmdsLists[] = { commandList };
	commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// apresenta frame e troca front/back buffer
	swapChain->Present(vSync, 0);
	backBufferIndex = (backBufferIndex + 1) % backBufferCount;

	// espera até que os comandos sejam executados pela GPU (ineficiente)
	WaitCommandQueue();
}

// -----------------------------------------------------------------------------

void Graphics::ResetCommands()
{
	// reinicia a lista de comandos para preparar para os comandos de inicialização
	commandList->Reset(commandListAlloc, nullptr);
}

// -----------------------------------------------------------------------------

void Graphics::SubmitCommands()
{
	// executa os comandos de inicialização
	commandList->Close();
	ID3D12CommandList* cmdsLists[] = { commandList };
	commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// espera até a inicialização completar
	WaitCommandQueue();
}

// -----------------------------------------------------------------------------
