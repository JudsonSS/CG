/**********************************************************************************
// Plane (Código Fonte)
//
// Criação:     22 Jul 2020
// Atualização: 17 Set 2021
// Compilador:  Visual C++ 2019
//
// Descrição:   Constrói um cubo usando o Direct3D 12
//
**********************************************************************************/

#include "Plane.h"

// ------------------------------------------------------------------------------

void Plane::Init()
{
    // contrói geometria e inicializa pipeline
    graphics->ResetCommands();
    // ---------------------------------------
    BuildGeometry();
    BuildRootSignature();
    BuildPipelineState();        
    // ---------------------------------------
    graphics->SubmitCommands();
}

// ------------------------------------------------------------------------------

void Plane::Update()
{
    // sai com o pressionamento da tecla ESC
    if (input->KeyPress(VK_ESCAPE))
        window->Close();
}

// ------------------------------------------------------------------------------

void Plane::Display()
{
    graphics->Clear(pipelineState);

    // submete comandos de configuração do pipeline
    graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
    graphics->CommandList()->IASetVertexBuffers(0, 1, geometry->VertexBufferView());
    graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // submete comandos de desenho
    graphics->CommandList()->DrawInstanced(6, 1, 0, 0);

    graphics->Present();    
}

// ------------------------------------------------------------------------------

void Plane::Finalize()
{
    rootSignature->Release();
    pipelineState->Release();
    delete geometry;
}


// ------------------------------------------------------------------------------
//                                     D3D                                      
// ------------------------------------------------------------------------------

void Plane::BuildGeometry()
{
    // vértices da geometria
    Vertex vertices[6] =
    {
        { XMFLOAT4(-0.5f, 0.5f, 0.0f, 1.0f), XMFLOAT4(Colors::Red) },
        { XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), XMFLOAT4(Colors::Red) },
        { XMFLOAT4(-0.5f, -0.5f, 0.0f, 1.0f), XMFLOAT4(Colors::Yellow) },
        { XMFLOAT4(0.5f, -0.5f, 0.0f, 1.0f), XMFLOAT4(Colors::Yellow) },
        { XMFLOAT4(-0.5f, -0.5f, 0.0f, 1.0f), XMFLOAT4(Colors::Yellow) },
        { XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), XMFLOAT4(Colors::Red) }
    };

    // ------------------------------------------------------------
    // --------> Transformação, Visualização e Projeção <----------
    // ------------------------------------------------------------

    //// matriz de mundo
    //XMMATRIX S = XMMatrixScaling(5.0f, 5.0f, 1.0f);
    //XMMATRIX R = XMMatrixRotationY(XMConvertToRadians(0));
    //XMMATRIX T = XMMatrixTranslation(0, 0, 1);
    //XMMATRIX W = S * R * T;

    //// campo de visão em graus
    //float fovAngle = 45;

    //// matriz de visualização
    //XMVECTOR pos = XMVectorSet(0, 0, -10, 1);
    //XMVECTOR target = XMVectorZero();
    //XMVECTOR up = XMVectorSet(0, 1, 0, 0);
    //XMMATRIX V = XMMatrixLookAtLH(pos, target, up);

    //// matriz de projeção
    //XMMATRIX P = XMMatrixPerspectiveFovLH(
    //    XMConvertToRadians(fovAngle), 
    //    window->AspectRatio(), 
    //    1.0f, 100.0f);

    //// matriz combinada mundo - câmera - projeção
    //XMMATRIX WorldViewProj = W * V * P;

    //// coloca vértices na janela de projeção
    //for (int i = 0; i < 6; ++i)
    //{
    //    XMVECTOR vertex = XMLoadFloat4(&vertices[i].Pos);
    //    XMVECTOR proj = XMVector3TransformCoord(vertex, WorldViewProj);
    //    XMStoreFloat4(&vertices[i].Pos, proj);
    //}

    // ------------------------------------------------------------

    // tamanho em bytes dos vértices
    const uint vbSize = 6 * sizeof(Vertex);

    // cria malha 3D
    geometry = new Mesh("Plane");

    // ajusta atributos da malha 3D
    geometry->vertexByteStride = sizeof(Vertex);
    geometry->vertexBufferSize = vbSize;

    // aloca recursos para o vertex buffer
    graphics->Allocate(vbSize, &geometry->vertexBufferCPU);
    graphics->Allocate(UPLOAD, vbSize, &geometry->vertexBufferUpload);
    graphics->Allocate(GPU, vbSize, &geometry->vertexBufferGPU);

    // copia vértices para o armazenamento local da malha
    graphics->Copy(vertices, vbSize, geometry->vertexBufferCPU);

    // copia vértices para o buffer da GPU usando o buffer de Upload
    graphics->Copy(vertices, vbSize, geometry->vertexBufferUpload, geometry->vertexBufferGPU);
}

// ------------------------------------------------------------------------------

void Plane::BuildRootSignature()
{
    // descrição para uma assinatura vazia
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.NumParameters = 0;
    rootSigDesc.pParameters = nullptr;
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

    // cria uma assinatura raiz vazia
    ThrowIfFailed(graphics->Device()->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature)));
}

// ------------------------------------------------------------------------------

void Plane::BuildPipelineState()
{
    // --------------------
    // --- Input Layout ---
    // --------------------
    
    D3D12_INPUT_ELEMENT_DESC inputLayout[2] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // --------------------
    // ----- Shaders ------
    // --------------------

    ID3DBlob* vertexShader;
    ID3DBlob* pixelShader;

    D3DReadFileToBlob(L"../x64/Debug/Vertex.cso", &vertexShader);
    D3DReadFileToBlob(L"../x64/Debug/Pixel.cso", &pixelShader);

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
        engine->window->Size(600, 600);
        engine->window->Color(30, 30, 30);
        engine->window->Title("Plane");
        engine->window->Icon(IDI_ICON);
        engine->window->Cursor(IDC_CURSOR);
        engine->window->LostFocus(Engine::Pause);
        engine->window->InFocus(Engine::Resume);

        // cria e executa a aplicação
        int exit = engine->Start(new Plane());

        // finaliza execução
        delete engine;
        return exit;
    }
    catch (Error & e)
    {
        // exibe mensagem em caso de erro
        MessageBox(nullptr, e.ToString().data(), "Plane", MB_OK);
        return 0;
    }
}

// ----------------------------------------------------------------------------

