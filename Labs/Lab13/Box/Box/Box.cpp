/**********************************************************************************
// Box (Código Fonte)
// 
// Criação:		27 Abr 2016
// Atualização:	16 Mai 2016
// Compilador:	Visual C++ 14
//
// Descrição:	Constrói um cubo 3D usando o Direct3D 12
//
**********************************************************************************/

#include "Box.h"

// ------------------------------------------------------------------------------

void Box::Init()
{
	// controla rotação do cubo
	theta = 1.5f * XM_PI;
	phi = XM_PIDIV4;
	radius = 5.0f;

	// pega última posição do mouse
	lastMousePosX = (float) input->MouseX();
	lastMousePosY = (float) input->MouseY();

	// inicializa as matrizes World e View para a identidade
	World = View = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };

	// inicializa a matriz de projeção
	float aspectRatio = window->Width() / float(window->Height());
	XMStoreFloat4x4(&Proj, XMMatrixPerspectiveFovLH(0.25f*XM_PI, aspectRatio, 1.0f, 100.0f));

	// contrói geometria e inicializa pipeline
	graphics->ResetCommands();
	// ---------------------------------------
	BuildDescriptorHeaps();
	BuildConstantBuffers();
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildBoxGeometry();
	BuildPSO();
	// ---------------------------------------
	graphics->SubmitCommands();
}

// ------------------------------------------------------------------------------

void Box::Update()
{
	if (input->KeyDown(VK_ESCAPE))
		window->Close();

	float mousePosX = (float)input->MouseX();
	float mousePosY = (float)input->MouseY();

	if (input->KeyDown(VK_LBUTTON))
	{
		// cada pixel corresponde a 1/4 de grau
		float dx = XMConvertToRadians(0.25f * static_cast<float>(mousePosX - lastMousePosX));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(mousePosY - lastMousePosY));

		// atualiza ângulos com base no deslocamento do mouse 
		// para orbitar a câmera ao redor da caixa
		theta += dx;
		phi += dy;

		// restringe o ângulo de phi
		phi = phi < 0.1f ? 0.1f : (phi > (XM_PI-0.1f) ? XM_PI - 0.1f : phi);
	}
	else if (input->KeyDown(VK_RBUTTON))
	{
		// cada pixel corresponde a 0.005 unidades
		float dx = 0.005f * static_cast<float>(mousePosX - lastMousePosX);
		float dy = 0.005f * static_cast<float>(mousePosY - lastMousePosY);

		// atualiza o raio da câmera com base no deslocamento do mouse 
		radius += dx - dy;

		// restringe o raio
		radius = radius < 3.0f ? 3.0f : (radius > 15.0f ? 15.0f : radius);
	}

	lastMousePosX = mousePosX;
	lastMousePosY = mousePosY;

	// converte coordenadas esféricas para cartesianas
	float x = radius * sinf(phi) * cosf(theta);
	float z = radius * sinf(phi) * sinf(theta);
	float y = radius * cosf(phi);

	// constrói a matriz da câmera (view matrix)
	XMVECTOR pos    = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view   = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&View, view);

	// constrói matrix de projeção combinada (world x view x proj)
	XMMATRIX world = XMLoadFloat4x4(&World);
	XMMATRIX proj = XMLoadFloat4x4(&Proj);
	XMMATRIX worldViewProj = world*view*proj;

	// atualiza o buffer constante com a matriz de projeção
	ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
	memcpy(constantBufferData, &objConstants, sizeof(ObjectConstants));
} 

// ------------------------------------------------------------------------------

void Box::Draw()
{	
	graphics->Clear(PSO);

	// configura pipeline do Direct 3D
	ID3D12DescriptorHeap * descriptorHeaps[] = { constantBufferHeap };
	graphics->CommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
	graphics->CommandList()->IASetVertexBuffers(0, 1, &boxGeo->VertexBufferView());
	graphics->CommandList()->IASetIndexBuffer(&boxGeo->IndexBufferView());
	graphics->CommandList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	graphics->CommandList()->SetGraphicsRootDescriptorTable(0, constantBufferHeap->GetGPUDescriptorHandleForHeapStart());
	graphics->CommandList()->DrawIndexedInstanced(boxGeo->SubMesh["box"].IndexCount, 1, 0, 0, 0);

	graphics->Present();
} 

// ------------------------------------------------------------------------------

void Box::Finalize()
{
	constantBufferUpload->Unmap(0, nullptr);
	constantBufferUpload->Release();
	constantBufferHeap->Release();
	vertexShaderByteCode->Release();
	pixelShaderByteCode->Release();
	rootSignature->Release();
	PSO->Release();
	delete boxGeo;
}

// ------------------------------------------------------------------------------

void Box::BuildDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC constantBufferHeapDesc = {};
	constantBufferHeapDesc.NumDescriptors = 1;
	constantBufferHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	constantBufferHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	graphics->Device()->CreateDescriptorHeap(&constantBufferHeapDesc, IID_PPV_ARGS(&constantBufferHeap));
}

// ------------------------------------------------------------------------------

void Box::BuildConstantBuffers()
{
	// propriedades da heap do upload buffer
	D3D12_HEAP_PROPERTIES uploadHeapProperties = {};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	uploadHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	uploadHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	uploadHeapProperties.CreationNodeMask = 1;
	uploadHeapProperties.VisibleNodeMask = 1;

	// o tamanho dos buffers constantes precisam ser um múltiplos 
	// do tamanho de alocação mínima do hardware (256 bytes)
	uint constantBufferSize = (sizeof(ObjectConstants) + 255) & ~255;

	// descrição do buffer de upload
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

	// endereço do buffer de upload na GPU
	D3D12_GPU_VIRTUAL_ADDRESS uploadAddress = constantBufferUpload->GetGPUVirtualAddress();

	// descreve view do buffer constante
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = uploadAddress;
	cbvDesc.SizeInBytes = constantBufferSize;

	// cria uma view para o buffer constante
	graphics->Device()->CreateConstantBufferView(
		&cbvDesc,
		constantBufferHeap->GetCPUDescriptorHandleForHeapStart());

	// mapeia memória do upload buffer para um endereço acessível pela CPU
	constantBufferUpload->Map(0, nullptr, reinterpret_cast<void**>(&constantBufferData));
}

// ------------------------------------------------------------------------------

void Box::BuildRootSignature()
{
	// cria uma única tabela de descritores de CBVs
	D3D12_DESCRIPTOR_RANGE cbvTable = {};
	cbvTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	cbvTable.NumDescriptors = 1;
	cbvTable.BaseShaderRegister = 0;
	cbvTable.RegisterSpace = 0;
	cbvTable.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// parâmetro raiz pode ser uma tabela, descritor raiz ou constantes raiz
	D3D12_ROOT_PARAMETER rootParameters[1];
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
	rootParameters[0].DescriptorTable.pDescriptorRanges = &cbvTable;

	// uma assinatura raiz é um vetor de parâmetros raiz
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.NumParameters = 1;
	rootSigDesc.pParameters = rootParameters;
	rootSigDesc.NumStaticSamplers = 0;
	rootSigDesc.pStaticSamplers = nullptr;
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// serializa assinatura raiz
	ID3DBlob * serializedRootSig = nullptr;
	ID3DBlob * errorBlob = nullptr;

	D3D12SerializeRootSignature(
		&rootSigDesc, 
		D3D_ROOT_SIGNATURE_VERSION_1,
		&serializedRootSig, 
		&errorBlob);

	if (errorBlob != nullptr)
	{
		OutputDebugString((char*)errorBlob->GetBufferPointer());
	}

	// cria uma assinatura raiz com um único slot que aponta para  
	// uma faixa de descritores consistindo de um único buffer constante
	graphics->Device()->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
}

// ------------------------------------------------------------------------------

void Box::BuildShadersAndInputLayout()
{
	// habilita depuração do código dos shaders
	UINT compileFlags = 0;
#ifdef _DEBUG
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	// carrega bytecode do vertex e pixel shaders
	D3DReadFileToBlob(L"../x64/Debug/Vertex.cso", &vertexShaderByteCode);
	D3DReadFileToBlob(L"../x64/Debug/Pixel.cso", &pixelShaderByteCode);

	// define layout de entrada dos vértices no pipeline
	inputLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputLayout[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
}

// ------------------------------------------------------------------------------

void Box::BuildBoxGeometry()
{
	// vértices da geometria
	Vertex vertices[8] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Red) },
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) },
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) },
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Red) },
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Yellow) },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }
	};

	// índices da geometria
	ushort indices[36] =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

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

	// tamanho em bytes dos vértices e índices
	const uint vbByteSize = 8 * sizeof(Vertex);
	const uint ibByteSize = 36 * sizeof(ushort);
	
	// cria malha 3D
	boxGeo = new Mesh("boxGeo");
	
	// copia vértices para o armazenamento local da malha
	D3DCreateBlob(vbByteSize, &boxGeo->VertexBufferCPU);
	CopyMemory(boxGeo->VertexBufferCPU->GetBufferPointer(), vertices, vbByteSize);

	// copia índices para o armazenamento local da malha 
	D3DCreateBlob(ibByteSize, &boxGeo->IndexBufferCPU);
	CopyMemory(boxGeo->IndexBufferCPU->GetBufferPointer(), indices, ibByteSize);

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
		IID_PPV_ARGS(&boxGeo->VertexBufferGPU));

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
		IID_PPV_ARGS(&boxGeo->VertexBufferUploader));

	// ------------------------------------------
	// Copia vértices para o buffer padrão (GPU)
	// ------------------------------------------

	// descreve os dados que serão copiados no buffer padrão
	D3D12_SUBRESOURCE_DATA vertexSubResourceData = {};
	vertexSubResourceData.pData = (const void *) vertices;
	vertexSubResourceData.RowPitch = vbByteSize;
	vertexSubResourceData.SlicePitch = vertexSubResourceData.RowPitch;

	// altera estado da memória na GPU para receber dados dos vértices
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = boxGeo->VertexBufferGPU;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	graphics->CommandList()->ResourceBarrier(1, &barrier);

	// copia dados para o buffer padrão (GPU)
	// primeiro copia-se os dados para a heap intermediária de upload
	// depois usando ID3D12CommandList::CopyBufferRegion copia-se do upload para a GPU
	ullong requiredSize = 0;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT layouts;
	uint numRows;
	ullong rowSizesInBytes;

	// pega layout da memória de um recurso (vertex buffer na GPU)
	graphics->Device()->GetCopyableFootprints(
		&boxGeo->VertexBufferGPU->GetDesc(), 
		0, 1, 0, &layouts, &numRows, 
		&rowSizesInBytes, &requiredSize);
	
	// trava memória do upload buffer para acesso exclusivo 
	BYTE * pData;
	boxGeo->VertexBufferUploader->Map(0, nullptr, (void**)&pData);

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
		BYTE * destSlice = (BYTE*)(DestData.pData) + DestData.SlicePitch * z;
		const BYTE * srcSlice = (const BYTE*)(vertexSubResourceData.pData) + vertexSubResourceData.SlicePitch * z;
		for (uint y = 0; y < numRows; ++y)
			memcpy(
				destSlice + DestData.RowPitch * y,	
				srcSlice + vertexSubResourceData.RowPitch * y, 
				(size_t) rowSizesInBytes);
	}
	
	// libera trava de memória do upload buffer 
	boxGeo->VertexBufferUploader->Unmap(0, nullptr);

	// copia vertex buffer do upload buffer para a GPU
	graphics->CommandList()->CopyBufferRegion(
		boxGeo->VertexBufferGPU, 
		0, 
		boxGeo->VertexBufferUploader, 
		layouts.Offset, 
		layouts.Footprint.Width);

	// altera estado da memória na GPU de escrita para leitura
	barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = boxGeo->VertexBufferGPU;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	graphics->CommandList()->ResourceBarrier(1, &barrier);


	// --------------------------------------
	// Cria um buffer padrão para os índices
	// --------------------------------------

	// descrição do buffer padrão
	D3D12_RESOURCE_DESC indexBufferGPUDesc = {};
	indexBufferGPUDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	indexBufferGPUDesc.Alignment = 0;
	indexBufferGPUDesc.Width = ibByteSize;
	indexBufferGPUDesc.Height = 1;
	indexBufferGPUDesc.DepthOrArraySize = 1;
	indexBufferGPUDesc.MipLevels = 1;
	indexBufferGPUDesc.Format = DXGI_FORMAT_UNKNOWN;
	indexBufferGPUDesc.SampleDesc.Count = 1;
	indexBufferGPUDesc.SampleDesc.Quality = 0;
	indexBufferGPUDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	indexBufferGPUDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// cria um buffer padrão para a geometria
	graphics->Device()->CreateCommittedResource(
		&defaultBufferProp,
		D3D12_HEAP_FLAG_NONE,
		&indexBufferGPUDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&boxGeo->IndexBufferGPU));

	// ------------------------------------------
	// Cria um buffer de upload para os índices
	// ------------------------------------------

	// descrição do buffer de upload
	D3D12_RESOURCE_DESC indexBufferUploadDesc = {};
	indexBufferUploadDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	indexBufferUploadDesc.Alignment = 0;
	indexBufferUploadDesc.Width = ibByteSize;
	indexBufferUploadDesc.Height = 1;
	indexBufferUploadDesc.DepthOrArraySize = 1;
	indexBufferUploadDesc.MipLevels = 1;
	indexBufferUploadDesc.Format = DXGI_FORMAT_UNKNOWN;
	indexBufferUploadDesc.SampleDesc.Count = 1;
	indexBufferUploadDesc.SampleDesc.Quality = 0;
	indexBufferUploadDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	indexBufferUploadDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// cria um buffer de upload para a geometria
	graphics->Device()->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&indexBufferUploadDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&boxGeo->IndexBufferUploader));

	// descreve os dados que serão copiados no buffer padrão
	D3D12_SUBRESOURCE_DATA indexSubResourceData = {};
	indexSubResourceData.pData = (const void *) indices;
	indexSubResourceData.RowPitch = ibByteSize;
	indexSubResourceData.SlicePitch = indexSubResourceData.RowPitch;

	// altera estado da memória na GPU para receber dados dos índices
	barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = boxGeo->IndexBufferGPU;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	graphics->CommandList()->ResourceBarrier(1, &barrier);

	// copia dados para o buffer padrão (GPU)
	// primeiro copia-se os dados para a heap intermediária de upload
	// depois usando ID3D12CommandList::CopyBufferRegion copia-se do upload para a GPU

	// pega layout da memória de um recurso (index buffer na GPU)
	graphics->Device()->GetCopyableFootprints(
		&boxGeo->IndexBufferGPU->GetDesc(), 
		0, 1, 0, &layouts, &numRows, 
		&rowSizesInBytes, &requiredSize);

	// trava memória do upload buffer para acesso exclusivo
	boxGeo->IndexBufferUploader->Map(0, nullptr, (void**)&pData);

	// descreve o destino de uma operação de cópia
	DestData = 
	{
		pData + layouts.Offset,
		layouts.Footprint.RowPitch,
		layouts.Footprint.RowPitch * numRows 
	};

	// copia índices no upload buffer
	for (uint z = 0; z < layouts.Footprint.Depth; ++z)
	{
		BYTE * destSlice = (BYTE*)(DestData.pData) + DestData.SlicePitch * z;
		const BYTE* srcSlice = (const BYTE*)(indexSubResourceData.pData) + indexSubResourceData.SlicePitch * z;
		for (uint y = 0; y < numRows; ++y)
			memcpy(
				destSlice + DestData.RowPitch * y, 
				srcSlice + indexSubResourceData.RowPitch * y, 
				(size_t) rowSizesInBytes);
	}

	// libera trava de memória do upload buffer
	boxGeo->IndexBufferUploader->Unmap(0, nullptr);

	// copia index buffer do upload buffer para a GPU
	graphics->CommandList()->CopyBufferRegion(
		boxGeo->IndexBufferGPU,
		0,
		boxGeo->IndexBufferUploader,
		layouts.Offset,
		layouts.Footprint.Width);

	// altera estado da memória na GPU de escrita para leitura
	barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = boxGeo->IndexBufferGPU;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	graphics->CommandList()->ResourceBarrier(1, &barrier);

	// -----------

	// ajusta atributos da malha 3D
	boxGeo->VertexByteStride = sizeof(Vertex);
	boxGeo->VertexBufferByteSize = vbByteSize;
	boxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	boxGeo->IndexBufferByteSize = ibByteSize;

	SubMesh boxSubMesh;
	boxSubMesh.IndexCount = 36;
	boxSubMesh.StartIndexLocation = 0;
	boxSubMesh.BaseVertexLocation = 0;

	// armazena informações da malha na tabela hash
	boxGeo->SubMesh["box"] = boxSubMesh;
}

// ------------------------------------------------------------------------------

void Box::BuildPSO()
{
	// descreve o estado do pipeline através de um Pipeline State Object (PSO)
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	
	psoDesc.InputLayout = { inputLayout, 2 };
	psoDesc.pRootSignature = rootSignature;
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(vertexShaderByteCode->GetBufferPointer()),
		vertexShaderByteCode->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(pixelShaderByteCode->GetBufferPointer()),
		pixelShaderByteCode->GetBufferSize()
	};

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

	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.DepthStencilState = depthStencilDesc;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = graphics->Antialiasing();
	psoDesc.SampleDesc.Quality = graphics->Quality();
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	graphics->Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&PSO));
}


// ------------------------------------------------------------------------------
//                                  WinMain                                      
// ------------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Engine * engine = new Engine();

	// configura a janela
	engine->window->Mode(WINDOWED);
	engine->window->Size(1024, 720);
	engine->window->Color(0, 122, 204);
	engine->window->Title("Box");
	engine->window->Icon(IDI_ICON);
	engine->window->Cursor(IDC_CURSOR);

	// configura dispositivo gráfico
	//engine->graphics->VSync(true);

	// inicia o jogo
	engine->Start(new Box());

	delete engine;
	return 0;
}

// ----------------------------------------------------------------------------

