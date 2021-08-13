/**********************************************************************************
// Graphics (C�digo Fonte)
// 
// Cria��o:     06 Abr 2011
// Atualiza��o: 13 Ago 2021
// Compilador:  Visual C++ 2019
//
// Descri��o:   Usa fun��es do Direct3D 12 para acessar a GPU
//
**********************************************************************************/

#include "Graphics.h"
#include "Error.h"
#include <sstream>
using std::wstringstream;

// ------------------------------------------------------------------------------

Graphics::Graphics()
{
    // configura��o
    backBufferCount = 2;    // double-buffering
    backBufferIndex = 0;    // zero � o primeiro
    antialiasing = 1;       // sem antialiasing
    quality = 0;            // qualidade padr�o
    vSync = false;          // sem vertical sync

    // cor de fundo
    bgColor[0] = 0.0f;      // Red
    bgColor[1] = 0.0f;      // Green
    bgColor[2] = 0.0f;      // Blue
    bgColor[3] = 0.0f;      // Alpha (0 = transparente, 1 = s�lido)

    // infraestrutura gr�fica
    factory           = nullptr;
    device            = nullptr;
    swapChain         = nullptr;
    commandQueue      = nullptr;
    commandList       = nullptr;
    commandListAlloc  = nullptr;
    
    // pipeline do Direct3D
    renderTargets     = new ID3D12Resource*[backBufferCount] {nullptr};
    depthStencil      = nullptr;
    renderTargetHeap  = nullptr;
    depthStencilHeap  = nullptr;
    rtDescriptorSize  = 0;
    ZeroMemory(&viewport, sizeof(viewport));
    ZeroMemory(&scissorRect, sizeof(scissorRect));

    // sincroniza��o cpu/gpu
    fence = nullptr;
    currentFence = 0;
}

// ------------------------------------------------------------------------------

Graphics::~Graphics()
{
    // espera GPU finalizar comandos na fila
    WaitCommandQueue();

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
        // Direct3D � incapaz de fechar quando em tela cheia
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

    // libera dispositivo gr�fico
    if (device)
        device->Release();

    // libera interface principal
    if (factory)
        factory->Release();
}

// ------------------------------------------------------------------------------

void Graphics::LogHardwareInfo()
{
    const uint BytesinMegaByte = 1048576U;

    // --------------------------------------
    // Adaptador de v�deo (placa de v�deo)
    // --------------------------------------
    IDXGIAdapter* adapter = nullptr;
    if (factory->EnumAdapters(0, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);

        wstringstream text;
        text << L"---> Placa de v�deo: " << desc.Description << L"\n";
        OutputDebugStringW(text.str().c_str());
    }

    IDXGIAdapter4* adapter4 = nullptr;
    if (SUCCEEDED(adapter->QueryInterface(IID_PPV_ARGS(&adapter4))))
    {
        DXGI_QUERY_VIDEO_MEMORY_INFO memInfo;
        adapter4->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memInfo);
        
        wstringstream text;
        text << L"---> Mem�ria de v�deo (livre): " << memInfo.Budget / BytesinMegaByte << L"MB\n";
        text << L"---> Mem�ria de v�deo (usada): " << memInfo.CurrentUsage / BytesinMegaByte << L"MB\n";
        OutputDebugStringW(text.str().c_str());

        adapter4->Release();
    }    

    // -----------------------------------------
    // Feature Level m�ximo suportado pela GPU
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

    // bloco de instru��es
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
    // Sa�da de v�deo (monitor)
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
    // Modo de v�deo (resolu��o)
    // ------------------------------------------

    // pega as dimens�es da tela
    uint dpi = GetDpiForSystem();
    uint screenWidth = GetSystemMetricsForDpi(SM_CXSCREEN, dpi);
    uint screenHeight = GetSystemMetricsForDpi(SM_CYSCREEN, dpi);

    // pega a frequencia de atualiza��o da tela
    DEVMODE devMode = { 0 };
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);
    uint refresh = devMode.dmDisplayFrequency;

    wstringstream text;
    text << L"---> Resolu��o: " << screenWidth << L"x" << screenHeight << L" " << refresh << L" Hz\n";
    OutputDebugStringW(text.str().c_str());

    // ------------------------------------------

    // libera interfaces DXGI utilizadas
    if (adapter) adapter->Release();
    if (output) output->Release();
}


// -----------------------------------------------------------------------------

void Graphics::Initialize(Window * window)
{
    // ---------------------------------------------------
    // Infraestrutura DXGI e o dispositivo D3D
    // ---------------------------------------------------

    uint factoryFlags = 0;

#ifdef _DEBUG
    // habilita a camada de depura��o do DXGI
    factoryFlags = DXGI_CREATE_FACTORY_DEBUG;

    // habilita a camada de depura��o do D3D12
    ID3D12Debug * debugController;
    ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
    debugController->EnableDebugLayer();
#endif

    // cria objeto para infraestrutura gr�fica do DirectX (DXGI)
    ThrowIfFailed(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory)));

    // cria objeto para dispositivo gr�fico
    if FAILED(D3D12CreateDevice(
        nullptr,                                // adaptador de v�deo (nullptr = adaptador padr�o)
        D3D_FEATURE_LEVEL_11_0,                 // vers�o m�nima dos recursos do Direct3D
        IID_PPV_ARGS(&device)))                 // guarda o dispositivo D3D criado
    {
        // tenta criar um dispositivo WARP 
        IDXGIAdapter * warp;
        ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warp)));

        // cria objeto D3D usando dispositivo WARP
        ThrowIfFailed(D3D12CreateDevice(
            warp,                               // adaptador de v�deo WARP (software)
            D3D_FEATURE_LEVEL_11_0,             // vers�o m�nima dos recursos do Direct3D
            IID_PPV_ARGS(&device)));            // guarda o dispositivo D3D criado

        // libera objeto n�o mais necess�rio
        warp->Release();

        // informa uso de um disposito WARP:
        // implementa as funcionalidades do 
        // D3D12 em software (lento)
        OutputDebugString("---> Usando Adaptador WARP: n�o h� suporte ao D3D12\n");
    }

    // exibe informa��es do hardware gr�fico no Output do Visual Studio
#ifdef _DEBUG
    LogHardwareInfo();
#endif 

    // ---------------------------------------------------
    // Fila, lista e alocador de commandos
    // ---------------------------------------------------

    // cria fila de comandos da GPU
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));

    // cria o alocador de comandos
    ThrowIfFailed(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&commandListAlloc)));

    // cria a lista de comandos
    ThrowIfFailed(device->CreateCommandList(
        0,                                      // usando apenas uma GPU
        D3D12_COMMAND_LIST_TYPE_DIRECT,         // n�o herda estado na GPU
        commandListAlloc,                       // alocador de comandos
        nullptr,                                // estado inicial do pipeline
        IID_PPV_ARGS(&commandList)));           // objeto lista de comandos

    // ---------------------------------------------------
    // Cerca para sincroniza��o da CPU/GPU
    // ---------------------------------------------------

    ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

    // ---------------------------------------------------
    // Swap Chain
    // ---------------------------------------------------

    // descreve swap chain
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = window->Width();
    swapChainDesc.Height = window->Height();
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.SampleDesc.Count = antialiasing;
    swapChainDesc.SampleDesc.Quality = quality;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = backBufferCount;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;    
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    
    // cria a swap chain
    ThrowIfFailed(factory->CreateSwapChainForHwnd(
        commandQueue,                           // fila de comandos da GPU 
        window->Id(),                           // identificador da janela
        &swapChainDesc,                         // descri��o da swap chain
        nullptr,                                // swap chain para tela cheia
        nullptr,                                // restringir tela de sa�da
        &swapChain));                           // objeto swap chain

    // ---------------------------------------------------
    // Render Target Views (e heaps associadas)
    // ---------------------------------------------------

    // descreve e cria uma heap para o descritor tipo Render Target (RT)
    D3D12_DESCRIPTOR_HEAP_DESC renderTargetHeapDesc = {};
    renderTargetHeapDesc.NumDescriptors = backBufferCount;
    renderTargetHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    renderTargetHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(device->CreateDescriptorHeap(&renderTargetHeapDesc, IID_PPV_ARGS(&renderTargetHeap)));

    // pega um Handle para o in�cio da Heap
    D3D12_CPU_DESCRIPTOR_HANDLE rtHandle = renderTargetHeap->GetCPUDescriptorHandleForHeapStart();

    // valor a incrementar para acessar o pr�ximo descritor dentro da Heap
    // o tamanho de um descritor depende do hardware gr�fico e do tipo de heap utilizada
    rtDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // cria um descritor (view) de Render Target para cada buffer (front e back buffers)
    for (uint i = 0; i < backBufferCount; ++i)
    {
        swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
        device->CreateRenderTargetView(renderTargets[i], nullptr, rtHandle);
        rtHandle.ptr += rtDescriptorSize;
    }

    // ---------------------------------------------------
    // Depth/Stencil View (e heaps associadas)
    // ---------------------------------------------------

    // descri��o do buffer Depth/Stencil
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

    // cria um buffer Depth/Stencil
    ThrowIfFailed(device->CreateCommittedResource(
        &dsHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_COMMON,
        &optmizedClear,
        IID_PPV_ARGS(&depthStencil)));

    // descreve e cria uma heap para o descritor tipo Depth/Stencil (DS)
    D3D12_DESCRIPTOR_HEAP_DESC depthstencilHeapDesc = {};
    depthstencilHeapDesc.NumDescriptors = 1;
    depthstencilHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    depthstencilHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(device->CreateDescriptorHeap(&depthstencilHeapDesc, IID_PPV_ARGS(&depthStencilHeap)));

    // pega um Handle para o in�cio da Heap
    D3D12_CPU_DESCRIPTOR_HANDLE dsHandle = depthStencilHeap->GetCPUDescriptorHandleForHeapStart();

    // cria um descritor (view) de Depth/Stencil para o mip n�vel 0
    device->CreateDepthStencilView(depthStencil, nullptr, dsHandle);

    // faz a transi��o do estado inicial do recurso para ser usado como buffer de profundidade
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = depthStencil;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &barrier);

    // submete comando para transi��o do depth/stencil buffer
    SubmitCommands();

    // ---------------------------------------------------
    // Viewport e Ret�ngulo de Recorte
    // ---------------------------------------------------

    // ajusta a viewport
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(window->Width());
    viewport.Height = static_cast<float>(window->Height());
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    // ajusta o ret�ngulo de corte
    scissorRect = { 0, 0, window->Width(), window->Height() };

    // ---------------------------------------------------
    // Cor de Fundo do Backbuffer
    // ---------------------------------------------------

    // ajusta a cor de fundo do backbuffer
    // para a mesma cor de fundo da janela
    COLORREF color = window->Color();

    bgColor[0] = GetRValue(color)/255.0f;   // Red
    bgColor[1] = GetGValue(color)/255.0f;   // Green
    bgColor[2] = GetBValue(color)/255.0f;   // Blue
    bgColor[3] = 1.0f;                      // Alpha (1 = s�lido)
}

// -----------------------------------------------------------------------------

void Graphics::Clear(ID3D12PipelineState * pso)
{
    // reutiliza a mem�ria associada com a lista de comandos
    // a lista de comandos deve ter terminado de executar na GPU
    commandListAlloc->Reset();

    // uma lista de comandos pode ser reinicializada depois de 
    // adicionada � fila de comandos da GPU (via ExecuteCommandList)
    commandList->Reset(commandListAlloc, pso);

    // indica que o backbuffer ser� usado como alvo de renderiza��o
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = renderTargets[backBufferIndex];
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &barrier);

    // ajusta a viewport e ret�ngulos de corte
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    // limpa o backbuffer e depth/stencil buffer    
    D3D12_CPU_DESCRIPTOR_HANDLE dsHandle = depthStencilHeap->GetCPUDescriptorHandleForHeapStart();
    D3D12_CPU_DESCRIPTOR_HANDLE rtHandle = renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
    rtHandle.ptr += SIZE_T(backBufferIndex) * SIZE_T(rtDescriptorSize);
    commandList->ClearRenderTargetView(rtHandle, bgColor, 0, nullptr);
    commandList->ClearDepthStencilView(dsHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    // especifica quais buffers ser�o utilizados na renderiza��o
    commandList->OMSetRenderTargets(1, &rtHandle, true, &dsHandle);
}

// ------------------------------------------------------------------------------

bool Graphics::WaitCommandQueue()
{
    // avan�a o valor da cerca para marcar novos comandos a partir desse ponto
    currentFence++;

    // adiciona uma instru��o na fila de comandos para inserir uma nova barreira
    // GPU vai finalizar todos os comandos em curso antes de processar esse sinal
    if (FAILED(commandQueue->Signal(fence, currentFence)))
        return false;

    // espera a GPU completar todos os comandos anteriores
    if (fence->GetCompletedValue() < currentFence)
    {
        HANDLE eventHandle = CreateEventEx(NULL, NULL, NULL, EVENT_ALL_ACCESS);

        if (eventHandle)
        {
            // aciona evento quando a GPU atingir a barreira atual  
            if (FAILED(fence->SetEventOnCompletion(currentFence, eventHandle)))
                return false;

            // espera at� o evento ser acionado
            WaitForSingleObject(eventHandle, INFINITE);
            CloseHandle(eventHandle);
        }
    }

    return true;
}

// -----------------------------------------------------------------------------

void Graphics::SubmitCommands()
{
    // submete os comandos gravados na lista para execu��o na GPU
    commandList->Close();
    ID3D12CommandList* cmdsLists[] = { commandList };
    commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // espera at� a GPU completar a execu��o dos comandos
    WaitCommandQueue();
}

// -----------------------------------------------------------------------------

void Graphics::Present()
{
    // indica que o backbuffer ser� usado para apresenta��o
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = renderTargets[backBufferIndex];
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &barrier);

    // submete a lista de comandos para execu��o na GPU
    SubmitCommands();

    // apresenta frame e troca front/back buffers
    swapChain->Present(vSync, 0);
    backBufferIndex = (backBufferIndex + 1) % backBufferCount;
}

// -----------------------------------------------------------------------------
