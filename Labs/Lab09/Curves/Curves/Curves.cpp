/**********************************************************************************
// Triangle (C�digo Fonte)
//
// Cria��o:		12 Ago 2020
// Atualiza��o:	12 Ago 2020
// Compilador:	Visual C++ 2019
//
// Descri��o:	Constr�i curvas usando corner-cutting no Direct3D 12
//
**********************************************************************************/

#include "Curves.h"

// ------------------------------------------------------------------------------

void Curves::Init()
{
	// aplica��o pausa/resume ao perder/ganhar o foco do usu�rio
	window->LostFocus(Engine::Pause);
	window->InFocus(Engine::Resume);

	numVertex = 0;
	index = 0;

	// contr�i geometria e inicializa pipeline
	graphics->ResetCommands();
	// ---------------------------------------
	BuildGeometry();
	BuildRootSignature();
	BuildPipelineState();		
	// ---------------------------------------
	graphics->SubmitCommands();
}

// ------------------------------------------------------------------------------

void Curves::Update()
{
	// sai com o pressionamento da tecla ESC
	if (input->KeyCtrl(VK_ESCAPE))
		window->Close();

	if (input->KeyCtrl(VK_LBUTTON))
	{
		float cx = float(window->CenterX());
		float cy = float(window->CenterY());
		float mx = float(input->MouseX());
		float my = float(input->MouseY());
		
		// converte as coordenadas da tela para a faixa -1.0 a 1.0
		// cy e my foram invertidos para levar em considera��o que 
		// o eixo y da tela cresce na dire��o oposta do cartesiano
		float x = (mx - cx) / cx;
		float y = (cy - my) / cy;

		vertices[index] = { XMFLOAT3(x, y, 0.0f), XMFLOAT4(Colors::White) };
		index = (index + 1) % MaxVertex;
		
		if (numVertex < MaxVertex)
			++numVertex;

		// copia v�rtices para o armazenamento local da malha
		graphics->Copy(vertices, geometry->VertexBufferSize, geometry->VertexBufferCPU);

		// copia v�rtices para o buffer da GPU usando o buffer de Upload
		graphics->ResetCommands();
		graphics->Copy(vertices, geometry->VertexBufferSize, geometry->VertexBufferUpload, geometry->VertexBufferGPU);
		graphics->SubmitCommands();
		Display();
	}
}

// ------------------------------------------------------------------------------

void Curves::Display()
{
	graphics->Clear(pipelineState);

	// submete comandos de configura��o do pipeline
	graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
	graphics->CommandList()->IASetVertexBuffers(0, 1, &geometry->VertexBufferView());
	graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);

	// submete comandos de desenho
	graphics->CommandList()->DrawInstanced(numVertex, 1, 0, 0);

	graphics->Present();	
}

// ------------------------------------------------------------------------------

void Curves::Finalize()
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

void Curves::BuildGeometry()
{
	// cria malha 3D
	geometry = new Mesh("Curve");

	// ajusta atributos da malha 3D
	geometry->VertexByteStride = sizeof(Vertex);
	geometry->VertexBufferSize = MaxVertex * sizeof(Vertex);

	// aloca recursos para o vertex buffer
	graphics->Allocate(geometry->VertexBufferSize, &geometry->VertexBufferCPU);
	graphics->Allocate(UPLOAD, geometry->VertexBufferSize, &geometry->VertexBufferUpload);
	graphics->Allocate(GPU, geometry->VertexBufferSize, &geometry->VertexBufferGPU);
}

// ------------------------------------------------------------------------------

void Curves::BuildRootSignature()
{
	// descri��o para uma assinatura vazia
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

void Curves::BuildPipelineState()
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

	D3DReadFileToBlob(L"../x64/Debug/Vertex.cso", &vertexShader);
	D3DReadFileToBlob(L"../x64/Debug/Pixel.cso", &pixelShader);

	// --------------------
	// ---- Rasterizer ----
	// --------------------

	D3D12_RASTERIZER_DESC rasterizer = {};
	rasterizer.FillMode = D3D12_FILL_MODE_WIREFRAME;
	rasterizer.CullMode = D3D12_CULL_MODE_NONE;
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
	pso.VS = { reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize()	};
	pso.PS = { reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
	pso.BlendState = blender;
	pso.SampleMask = UINT_MAX;
	pso.RasterizerState = rasterizer;
	pso.DepthStencilState = depthStencil;
	pso.InputLayout = { inputLayout, 2 };
	pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	pso.NumRenderTargets = 1;
	pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pso.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	pso.SampleDesc.Count = graphics->Antialiasing();
	pso.SampleDesc.Quality = graphics->Quality();
	graphics->Device()->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipelineState));
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
		engine->window->Title("Curves");
		engine->window->Icon(IDI_ICON);
		engine->window->Cursor(IDC_CURSOR);

		// cria e executa a aplica��o
		int exit = engine->Start(new Curves());

		// finaliza execu��o
		delete engine;
		return exit;
	}
	catch (Error & e)
	{
		// exibe mensagem em caso de erro
		MessageBox(nullptr, e.ToString().data(), "Curves", MB_OK);
		return 0;
	}
}

// ----------------------------------------------------------------------------

