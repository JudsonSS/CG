/**********************************************************************************
// Triangle (Código Fonte)
//
// Criação:		22 Jul 2020
// Atualização:	26 Jul 2020
// Compilador:	Visual C++ 2019
//
// Descrição:	Constrói um Triângulo usando o Direct3D 12
//
**********************************************************************************/

#include "Triangle.h"

// ------------------------------------------------------------------------------

void Triangle::Init()
{
	// aplicação pausa/resume ao perder/ganhar o foco do usuário
	window->LostFocus(Engine::Pause);
	window->InFocus(Engine::Resume);

	// contrói geometria e inicializa pipeline
	graphics->ResetCommands();
	// ---------------------------------------
	BuildRootSignature();
	BuildInputLayout();
	BuildShaders();
	BuildPipelineState();
	BuildGeometry();	
	// ---------------------------------------
	graphics->SubmitCommands();
}

// ------------------------------------------------------------------------------

void Triangle::Update()
{
	// sai com o pressionamento da tecla ESC
	if (input->KeyCtrl(VK_ESCAPE))
		window->Close();
}

// ------------------------------------------------------------------------------

void Triangle::Display()
{
	graphics->Clear(pipelineState);

	// submete comandos de configuração do pipeline
	graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
	graphics->CommandList()->IASetVertexBuffers(0, 1, &geometry->VertexBufferView());
	graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// submete comandos de desenho
	graphics->CommandList()->DrawInstanced(3, 1, 0, 0);

	graphics->Present();	
}

// ------------------------------------------------------------------------------

void Triangle::Finalize()
{
	vertexShader->Release();
	pixelShader->Release();
	rootSignature->Release();
	pipelineState->Release();
	delete geometry;
}


// ------------------------------------------------------------------------------
//                                     D3D                                      
// ------------------------------------------------------------------------------

void Triangle::BuildRootSignature()
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

void Triangle::BuildInputLayout()
{
	// define layout de entrada dos vértices no pipeline
	inputLayout[0] = 
	{ 
		"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 
	};
	
	inputLayout[1] = 
	{ 
		"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, 
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 
	};
}

// ------------------------------------------------------------------------------

void Triangle::BuildShaders()
{
	// carrega bytecode do vertex e pixel shaders
	D3DReadFileToBlob(L"../x64/Debug/Vertex.cso", &vertexShader);
	D3DReadFileToBlob(L"../x64/Debug/Pixel.cso", &pixelShader);
}

// ------------------------------------------------------------------------------

void Triangle::BuildPipelineState()
{
	// --------------------
	// --- Rasterizador ---
	// --------------------
	D3D12_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.ForcedSampleCount = 0;
	rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	// ---------------------------
	// --- Misturador de Cores ---
	// ---------------------------
	D3D12_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
	{
		FALSE,FALSE,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP,
		D3D12_COLOR_WRITE_ENABLE_ALL,
	};
	for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		blendDesc.RenderTarget[i] = defaultRenderTargetBlendDesc;

	// ------------------------------
	// --- Buffer de Profundidade ---
	// ------------------------------
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
	{ D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
	depthStencilDesc.FrontFace = defaultStencilOp;
	depthStencilDesc.BackFace = defaultStencilOp;
	

	// configura o pipeline através de um Pipeline State Object (PSO)
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = rootSignature;
	psoDesc.VS = { reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize()	};
	psoDesc.PS = { reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
	psoDesc.BlendState = blendDesc;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.DepthStencilState = depthStencilDesc;
	psoDesc.InputLayout = { inputLayout, 2 };
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = graphics->Antialiasing();
	psoDesc.SampleDesc.Quality = graphics->Quality();
	graphics->Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));
}

// ------------------------------------------------------------------------------

void Triangle::BuildGeometry()
{
	float aspectRatio = window->Width() / float(window->Height());

	// vértices da geometria
	Vertex vertices[3] =
	{
		{ XMFLOAT3(0.0f, 0.25f * aspectRatio, 0.0f), XMFLOAT4(Colors::Red) },
		{ XMFLOAT3(0.25f, -0.25f * aspectRatio, 0.0f), XMFLOAT4(Colors::Orange) },
		{ XMFLOAT3(-0.25f, -0.25f * aspectRatio, 0.0f), XMFLOAT4(Colors::Yellow) }
	};

	// tamanho em bytes dos vértices
	const uint vbSize = 8 * sizeof(Vertex);

	// cria malha 3D
	geometry = new Mesh("Geometry");

	// ajusta atributos da malha 3D
	geometry->VertexByteStride = sizeof(Vertex);
	geometry->VertexBufferSize = vbSize;

	// aloca recursos para o vertex buffer
	graphics->Allocate(vbSize, &geometry->VertexBufferCPU);
	graphics->Allocate(UPLOAD, vbSize, &geometry->VertexBufferUpload);
	graphics->Allocate(GPU, vbSize, &geometry->VertexBufferGPU);

	// copia vértices para o armazenamento local da malha
	graphics->Copy(vertices, vbSize, geometry->VertexBufferCPU);

	// copia vértices para o buffer da GPU usando o buffer de Upload
	graphics->Copy(vertices, vbSize, geometry->VertexBufferUpload, geometry->VertexBufferGPU);
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
		engine->window->Size(1024, 600);
		engine->window->Color(0, 122, 204);
		engine->window->Title("Triangle");
		engine->window->Icon(IDI_ICON);
		engine->window->Cursor(IDC_CURSOR);

		// cria e executa a aplicação
		int exit = engine->Start(new Triangle());

		// finaliza execução
		delete engine;
		return exit;
	}
	catch (Error & e)
	{
		// exibe mensagem em caso de erro
		MessageBox(nullptr, e.ToString().data(), "Triangle", MB_OK);
		return 0;
	}
}

// ----------------------------------------------------------------------------

