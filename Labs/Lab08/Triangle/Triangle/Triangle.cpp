/**********************************************************************************
// Triangle (Código Fonte)
//
// Criação:		22 Jul 2020
// Atualização:	22 Jul 2020
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
	BuildGeometry();
	BuildPSO();
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

void Triangle::Draw()
{
	graphics->Clear(PSO);

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
	vertexShaderByteCode->Release();
	pixelShaderByteCode->Release();
	rootSignature->Release();
	PSO->Release();
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
	D3DReadFileToBlob(L"../x64/Debug/Vertex.cso", &vertexShaderByteCode);
	D3DReadFileToBlob(L"../x64/Debug/Pixel.cso", &pixelShaderByteCode);
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
	const uint vbByteSize = 8 * sizeof(Vertex);

	// cria malha 3D
	geometry = new Mesh("Geometry");

	// copia vértices para o armazenamento local da malha
	D3DCreateBlob(vbByteSize, &geometry->VertexBufferCPU);
	CopyMemory(geometry->VertexBufferCPU->GetBufferPointer(), vertices, vbByteSize);

	// --------------------------------------
	// Cria um buffer padrão para os vértices
	// --------------------------------------

	// propriedades da heap do buffer padrão
	D3D12_HEAP_PROPERTIES defaultBufferProp = {};
	defaultBufferProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	defaultBufferProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	defaultBufferProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	defaultBufferProp.CreationNodeMask = 1;
	defaultBufferProp.VisibleNodeMask = 1;

	// descrição do buffer padrão
	D3D12_RESOURCE_DESC vertexBufferGPUDesc = {};
	vertexBufferGPUDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexBufferGPUDesc.Alignment = 0;
	vertexBufferGPUDesc.Width = vbByteSize;
	vertexBufferGPUDesc.Height = 1;
	vertexBufferGPUDesc.DepthOrArraySize = 1;
	vertexBufferGPUDesc.MipLevels = 1;
	vertexBufferGPUDesc.Format = DXGI_FORMAT_UNKNOWN;
	vertexBufferGPUDesc.SampleDesc.Count = 1;
	vertexBufferGPUDesc.SampleDesc.Quality = 0;
	vertexBufferGPUDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	vertexBufferGPUDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// cria um buffer padrão para a geometria
	graphics->Device()->CreateCommittedResource(
		&defaultBufferProp,
		D3D12_HEAP_FLAG_NONE,
		&vertexBufferGPUDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&geometry->VertexBufferGPU));

	// ------------------------------------------
	// Cria um buffer de upload para os vértices
	// ------------------------------------------

	// propriedades da Heap do buffer de upload
	D3D12_HEAP_PROPERTIES uploadHeapProperties = {};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	uploadHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	uploadHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	uploadHeapProperties.CreationNodeMask = 1;
	uploadHeapProperties.VisibleNodeMask = 1;

	// descrição do buffer de upload
	D3D12_RESOURCE_DESC vertexBufferUploadDesc = {};
	vertexBufferUploadDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexBufferUploadDesc.Alignment = 0;
	vertexBufferUploadDesc.Width = vbByteSize;
	vertexBufferUploadDesc.Height = 1;
	vertexBufferUploadDesc.DepthOrArraySize = 1;
	vertexBufferUploadDesc.MipLevels = 1;
	vertexBufferUploadDesc.Format = DXGI_FORMAT_UNKNOWN;
	vertexBufferUploadDesc.SampleDesc.Count = 1;
	vertexBufferUploadDesc.SampleDesc.Quality = 0;
	vertexBufferUploadDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	vertexBufferUploadDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// cria um buffer de upload para a geometria
	graphics->Device()->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vertexBufferUploadDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&geometry->VertexBufferUploader));

	// ------------------------------------------
	// Copia vértices para o buffer padrão (GPU)
	// ------------------------------------------

	// descreve os dados que serão copiados para o buffer padrão
	D3D12_SUBRESOURCE_DATA vertexSubResourceData = {};
	vertexSubResourceData.pData = (const void*)vertices;
	vertexSubResourceData.RowPitch = vbByteSize;
	vertexSubResourceData.SlicePitch = vertexSubResourceData.RowPitch;

	// altera estado da memória na GPU para receber dados dos vértices
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = geometry->VertexBufferGPU;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	graphics->CommandList()->ResourceBarrier(1, &barrier);

	// copia dados para o buffer padrão (GPU)
	// primeiro copia-se os dados para a heap intermediária de upload
	// depois usando ID3D12CommandList::CopyBufferRegion copia-se de upload para a GPU
	ullong requiredSize = 0;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT layouts;
	uint numRows;
	ullong rowSizesInBytes;

	// pega layout da memória de um recurso (vertex buffer na GPU)
	graphics->Device()->GetCopyableFootprints(
		&geometry->VertexBufferGPU->GetDesc(),
		0, 1, 0, &layouts, &numRows,
		&rowSizesInBytes, &requiredSize);

	// trava memória do upload buffer para acesso exclusivo 
	BYTE* pData;
	geometry->VertexBufferUploader->Map(0, nullptr, (void**)&pData);

	// descreve o destino de uma operação de cópia
	D3D12_MEMCPY_DEST DestData =
	{
		pData + layouts.Offset,
		layouts.Footprint.RowPitch,
		layouts.Footprint.RowPitch * numRows
	};

	// copia vértices no upload buffer
	for (uint z = 0; z < layouts.Footprint.Depth; ++z)
	{
		BYTE* destSlice = (BYTE*)(DestData.pData) + DestData.SlicePitch * z;
		const BYTE* srcSlice = (const BYTE*)(vertexSubResourceData.pData) + vertexSubResourceData.SlicePitch * z;
		for (uint y = 0; y < numRows; ++y)
			memcpy(
				destSlice + DestData.RowPitch * y,
				srcSlice + vertexSubResourceData.RowPitch * y,
				(size_t)rowSizesInBytes);
	}

	// libera trava de memória do upload buffer 
	geometry->VertexBufferUploader->Unmap(0, nullptr);

	// copia vertex buffer do upload buffer para a GPU
	graphics->CommandList()->CopyBufferRegion(
		geometry->VertexBufferGPU,
		0,
		geometry->VertexBufferUploader,
		layouts.Offset,
		layouts.Footprint.Width);

	// altera estado da memória na GPU de escrita para leitura
	barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = geometry->VertexBufferGPU;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	graphics->CommandList()->ResourceBarrier(1, &barrier);

	// -----------

	// ajusta atributos da malha 3D
	geometry->VertexByteStride = sizeof(Vertex);
	geometry->VertexBufferByteSize = vbByteSize;

	SubMesh boxSubMesh;
	boxSubMesh.BaseVertexLocation = 0;

	// armazena informações na tabela hash da malha
	geometry->SubMesh["box"] = boxSubMesh;
}

// ------------------------------------------------------------------------------

void Triangle::BuildPSO()
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
	psoDesc.VS = { reinterpret_cast<BYTE*>(vertexShaderByteCode->GetBufferPointer()), vertexShaderByteCode->GetBufferSize()	};
	psoDesc.PS = { reinterpret_cast<BYTE*>(pixelShaderByteCode->GetBufferPointer()), pixelShaderByteCode->GetBufferSize() };
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
	graphics->Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&PSO));
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
		int exitCode = engine->Start(new Triangle());

		// finaliza execução
		delete engine;
		return exitCode;
	}
	catch (Error & e)
	{
		// exibe mensagem em caso de erro
		MessageBox(nullptr, e.ToString().data(), "Triangle", MB_OK);
		return 0;
	}
}

// ----------------------------------------------------------------------------

