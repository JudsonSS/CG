/**********************************************************************************
// Camera (C�digo Fonte)
//
// Cria��o:		27 Abr 2016
// Atualiza��o:	26 Set 2021
// Compilador:	Visual C++ 2019
//
// Descri��o:	Controla a c�mera em uma cena 3D
//
**********************************************************************************/

#include "Camera.h"

// ------------------------------------------------------------------------------

void Camera::Init()
{
    // controla rota��o do cubo
    theta = XM_PIDIV4;
    phi = XM_PIDIV4;
    radius = 5.0f;

    // pega �ltima posi��o do mouse
    lastMousePosX = (float) input->MouseX();
    lastMousePosY = (float) input->MouseY();

    // inicializa as matrizes World e View para a identidade
    World = View = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f };

    // inicializa a matriz de proje��o
    XMStoreFloat4x4(&Proj, XMMatrixPerspectiveFovLH(
        XMConvertToRadians(45.0f), 
        window->AspectRatio(), 
        1.0f, 100.0f));

    // contr�i geometria e inicializa pipeline
    graphics->ResetCommands();
    // ---------------------------------------
    BuildConstantBuffers();
    BuildGeometry();
    BuildRootSignature();
    BuildPipelineState();        
    // ---------------------------------------
    graphics->SubmitCommands();
}

// ------------------------------------------------------------------------------

void Camera::Update()
{
    // sai com o pressionamento da tecla ESC
    if (input->KeyPress(VK_ESCAPE))
        window->Close();

    float mousePosX = (float) input->MouseX();
    float mousePosY = (float) input->MouseY();

    if (input->KeyDown(VK_LBUTTON))
    {
        // cada pixel corresponde a 1/4 de grau
        float dx = XMConvertToRadians(0.25f * static_cast<float>(mousePosX - lastMousePosX));
        float dy = XMConvertToRadians(0.25f * static_cast<float>(mousePosY - lastMousePosY));

        // atualiza �ngulos com base no deslocamento do mouse 
        // para orbitar a c�mera ao redor da caixa
        theta += dx;
        phi += dy;

        // restringe o �ngulo de phi
        phi = phi < 0.1f ? 0.1f : (phi > (XM_PI - 0.1f) ? XM_PI - 0.1f : phi);
    }
    else if (input->KeyDown(VK_RBUTTON))
    {
        // cada pixel corresponde a 0.005 unidades
        float dx = 0.005f * static_cast<float>(mousePosX - lastMousePosX);
        float dy = 0.005f * static_cast<float>(mousePosY - lastMousePosY);

        // atualiza o raio da c�mera com base no deslocamento do mouse 
        radius += dx - dy;

        // restringe o raio
        radius = radius < 3.0f ? 3.0f : (radius > 15.0f ? 15.0f : radius);
    }

    lastMousePosX = mousePosX;
    lastMousePosY = mousePosY;

    // converte coordenadas esf�ricas para cartesianas
    float x = radius * sinf(phi) * cosf(theta);
    float z = radius * sinf(phi) * sinf(theta);
    float y = radius * cosf(phi);

    // constr�i a matriz da c�mera (view matrix)
    XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
    XMStoreFloat4x4(&View, view);

    // constr�i matriz combinada (world x view x proj)
    XMMATRIX world = XMLoadFloat4x4(&World);
    XMMATRIX proj = XMLoadFloat4x4(&Proj);
    XMMATRIX WorldViewProj = world * view * proj;

    // atualiza o buffer constante com a matriz combinada
    ObjectConstants objConstants;
    XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(WorldViewProj));
    memcpy(constantBufferData, &objConstants, sizeof(ObjectConstants));
}

// ------------------------------------------------------------------------------

void Camera::Draw()
{
    // limpa o backbuffer
    graphics->Clear(pipelineState);

    // comandos de configura��o do pipeline
    ID3D12DescriptorHeap* descriptorHeaps[] = { constantBufferHeap };
    graphics->CommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
    graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
    graphics->CommandList()->IASetVertexBuffers(0, 1, geometry->VertexBufferView());
    graphics->CommandList()->IASetIndexBuffer(geometry->IndexBufferView());
    graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    graphics->CommandList()->SetGraphicsRootDescriptorTable(0, constantBufferHeap->GetGPUDescriptorHandleForHeapStart());

    // comando de desenho
    graphics->CommandList()->DrawIndexedInstanced(36, 1, 0, 0, 0);
 
    // apresenta o backbuffer na tela
    graphics->Present();    
}

// ------------------------------------------------------------------------------

void Camera::Finalize()
{
    constantBufferUpload->Unmap(0, nullptr);
    constantBufferUpload->Release();
    constantBufferHeap->Release();

    rootSignature->Release();
    pipelineState->Release();
    delete geometry;
}


// ------------------------------------------------------------------------------
//                                     D3D                                      
// ------------------------------------------------------------------------------

void Camera::BuildConstantBuffers()
{
    // descritor do buffer constante
    D3D12_DESCRIPTOR_HEAP_DESC constantBufferHeapDesc = {};
    constantBufferHeapDesc.NumDescriptors = 1;
    constantBufferHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    constantBufferHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    // cria descritor para buffer constante
    graphics->Device()->CreateDescriptorHeap(&constantBufferHeapDesc, IID_PPV_ARGS(&constantBufferHeap));

    // propriedades da heap do buffer de upload
    D3D12_HEAP_PROPERTIES uploadHeapProperties = {};
    uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
    uploadHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    uploadHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    uploadHeapProperties.CreationNodeMask = 1;
    uploadHeapProperties.VisibleNodeMask = 1;

    // o tamanho dos "Constant Buffers" precisam ser m�ltiplos 
    // do tamanho de aloca��o m�nima do hardware (256 bytes)
    uint constantBufferSize = (sizeof(ObjectConstants) + 255) & ~255;

    // descri��o do buffer de upload
    D3D12_RESOURCE_DESC uploadBufferDesc = {};
    uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    uploadBufferDesc.Alignment = 0;
    uploadBufferDesc.Width = constantBufferSize;
    uploadBufferDesc.Height = 1;
    uploadBufferDesc.DepthOrArraySize = 1;
    uploadBufferDesc.MipLevels = 1;
    uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    uploadBufferDesc.SampleDesc.Count = 1;
    uploadBufferDesc.SampleDesc.Quality = 0;
    uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    // cria um buffer de upload para o buffer constante
    graphics->Device()->CreateCommittedResource(
        &uploadHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &uploadBufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&constantBufferUpload));

    // endere�o do buffer de upload na GPU
    D3D12_GPU_VIRTUAL_ADDRESS uploadAddress = constantBufferUpload->GetGPUVirtualAddress();

    // descreve view do buffer constante
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
    cbvDesc.BufferLocation = uploadAddress;
    cbvDesc.SizeInBytes = constantBufferSize;

    // cria uma view para o buffer constante
    graphics->Device()->CreateConstantBufferView(
        &cbvDesc,
        constantBufferHeap->GetCPUDescriptorHandleForHeapStart());

    // mapeia mem�ria do upload buffer para um endere�o acess�vel pela CPU
    constantBufferUpload->Map(0, nullptr, reinterpret_cast<void**>(&constantBufferData));
}

void Camera::BuildGeometry()
{
    // v�rtices da geometria
    Vertex vertices[8] =
    {
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Red) },
        { XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Yellow) },
        { XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Yellow) },
        { XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Red) },
        { XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Yellow) },
        { XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Red) },
        { XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Red) },
        { XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }
    };

    // �ndices da geometria
    ushort indices[36] =
    {
        // front face
        0, 1, 3,
        1, 2, 3,

        // back face
        4, 6, 5,
        4, 7, 6,

        // left face
        4, 5, 1,
        4, 1, 0,

        // right face
        3, 2, 6,
        3, 6, 7,

        // top face
        1, 5, 6,
        1, 6, 2,

        // bottom face
        4, 0, 3,
        4, 3, 7
    };

    // -----------------------------------------------------------
    // >> Aloca��o e C�pia de Vertex e Index Buffers para a GPU <<
    // -----------------------------------------------------------

    // tamanho em bytes dos v�rtices e �ndices
    const uint vbSize = 8 * sizeof(Vertex);
    const uint ibSize = 36 * sizeof(ushort);

    // cria malha 3D
    geometry = new Mesh("Box");

    // ajusta atributos da malha 3D
    geometry->vertexByteStride = sizeof(Vertex);
    geometry->vertexBufferSize = vbSize;
    geometry->indexFormat = DXGI_FORMAT_R16_UINT;
    geometry->indexBufferSize = ibSize;

    // aloca recursos para o vertex buffer
    graphics->Allocate(vbSize, &geometry->vertexBufferCPU);
    graphics->Allocate(UPLOAD, vbSize, &geometry->vertexBufferUpload);
    graphics->Allocate(GPU, vbSize, &geometry->vertexBufferGPU);

    // aloca recursos para o index buffer
    graphics->Allocate(ibSize, &geometry->indexBufferCPU);
    graphics->Allocate(UPLOAD, ibSize, &geometry->indexBufferUpload);
    graphics->Allocate(GPU, ibSize, &geometry->indexBufferGPU);

    // guarda uma c�pia dos v�rtices e �ndices na malha
    graphics->Copy(vertices, vbSize, geometry->vertexBufferCPU);
    graphics->Copy(indices, ibSize, geometry->indexBufferCPU);

    // copia v�rtices e �ndices para a GPU usando o buffer de Upload
    graphics->Copy(vertices, vbSize, geometry->vertexBufferUpload, geometry->vertexBufferGPU);
    graphics->Copy(indices, ibSize, geometry->indexBufferUpload, geometry->indexBufferGPU);
}

// ------------------------------------------------------------------------------

void Camera::BuildRootSignature()
{
    // cria uma �nica tabela de descritores de CBVs
    D3D12_DESCRIPTOR_RANGE cbvTable = {};
    cbvTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    cbvTable.NumDescriptors = 1;
    cbvTable.BaseShaderRegister = 0;
    cbvTable.RegisterSpace = 0;
    cbvTable.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // par�metro raiz pode ser uma tabela, descritor raiz ou constantes raiz
    D3D12_ROOT_PARAMETER rootParameters[1];
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
    rootParameters[0].DescriptorTable.pDescriptorRanges = &cbvTable;

    // uma assinatura raiz � um vetor de par�metros raiz
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.NumParameters = 1;
    rootSigDesc.pParameters = rootParameters;
    rootSigDesc.NumStaticSamplers = 0;
    rootSigDesc.pStaticSamplers = nullptr;
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    // serializa assinatura raiz
    ID3DBlob* serializedRootSig = nullptr;
    ID3DBlob* error = nullptr;

    ThrowIfFailed(D3D12SerializeRootSignature(
        &rootSigDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &serializedRootSig,
        &error));

    if (error != nullptr)
    {
        OutputDebugString((char*)error->GetBufferPointer());
    }

    // cria uma assinatura raiz com um �nico slot que aponta para  
    // uma faixa de descritores consistindo de um �nico buffer constante
    ThrowIfFailed(graphics->Device()->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature)));
}

// ------------------------------------------------------------------------------

void Camera::BuildPipelineState()
{
    // --------------------
    // --- Input Layout ---
    // --------------------
    
    D3D12_INPUT_ELEMENT_DESC inputLayout[2] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // --------------------
    // ----- Shaders ------
    // --------------------

    ID3DBlob* vertexShader;
    ID3DBlob* pixelShader;

    D3DReadFileToBlob(L"Shaders/Vertex.cso", &vertexShader);
    D3DReadFileToBlob(L"Shaders/Pixel.cso", &pixelShader);

    // --------------------
    // ---- Rasterizer ----
    // --------------------

    D3D12_RASTERIZER_DESC rasterizer = {};
    rasterizer.FillMode = D3D12_FILL_MODE_SOLID;
    //rasterizer.FillMode = D3D12_FILL_MODE_WIREFRAME;
    rasterizer.CullMode = D3D12_CULL_MODE_BACK;
    rasterizer.FrontCounterClockwise = FALSE;
    rasterizer.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizer.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizer.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizer.DepthClipEnable = TRUE;
    rasterizer.MultisampleEnable = FALSE;
    rasterizer.AntialiasedLineEnable = FALSE;
    rasterizer.ForcedSampleCount = 0;
    rasterizer.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    // ---------------------
    // --- Color Blender ---
    // ---------------------

    D3D12_BLEND_DESC blender = {};
    blender.AlphaToCoverageEnable = FALSE;
    blender.IndependentBlendEnable = FALSE;
    const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
    {
        FALSE,FALSE,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };
    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
        blender.RenderTarget[i] = defaultRenderTargetBlendDesc;

    // ---------------------
    // --- Depth Stencil ---
    // ---------------------

    D3D12_DEPTH_STENCIL_DESC depthStencil = {};
    depthStencil.DepthEnable = TRUE;
    depthStencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencil.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    depthStencil.StencilEnable = FALSE;
    depthStencil.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    depthStencil.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
    { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
    depthStencil.FrontFace = defaultStencilOp;
    depthStencil.BackFace = defaultStencilOp;
    
    // -----------------------------------
    // --- Pipeline State Object (PSO) ---
    // -----------------------------------

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
    pso.pRootSignature = rootSignature;
    pso.VS = { reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
    pso.PS = { reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
    pso.BlendState = blender;
    pso.SampleMask = UINT_MAX;
    pso.RasterizerState = rasterizer;
    pso.DepthStencilState = depthStencil;
    pso.InputLayout = { inputLayout, 2 };
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso.NumRenderTargets = 1;
    pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pso.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    pso.SampleDesc.Count = graphics->Antialiasing();
    pso.SampleDesc.Quality = graphics->Quality();
    graphics->Device()->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipelineState));

    vertexShader->Release();
    pixelShader->Release();
}


// ------------------------------------------------------------------------------
//                                  WinMain                                      
// ------------------------------------------------------------------------------

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
    try
    {
        // cria motor e configura a janela
        Engine* engine = new Engine();
        engine->window->Mode(WINDOWED);
        engine->window->Size(800, 600);
        engine->window->Color(30, 30, 30);
        engine->window->Title("C�mera");
        engine->window->Icon(IDI_ICON);
        engine->window->Cursor(IDC_CURSOR);
        engine->window->LostFocus(Engine::Pause);
        engine->window->InFocus(Engine::Resume);

        // cria e executa a aplica��o
        int exit = engine->Start(new Camera());

        // finaliza execu��o
        delete engine;
        return exit;
    }
    catch (Error & e)
    {
        // exibe mensagem em caso de erro
        MessageBox(nullptr, e.ToString().data(), "C�mera", MB_OK);
        return 0;
    }
}

// ----------------------------------------------------------------------------

