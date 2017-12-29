#include "DirectX11Renderer.h"
#include <sstream>


#include "FastDelegate.h"
#include "FastDelegateBind.h"

#include "ResolutionChangedEventData.h"

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")

#define VIEWPORT_DEPTH_MIN 0.0f
#define VIEWPORT_DEPTH_MAX 1.0f

#define VIEW_NEAR 1.0f
#define VIEW_FAR 1000.0f
extern HWND g_Window;

using namespace fastdelegate;

void
DirectX11Renderer::Init()
{
	InitEventHandlers();
	InitDirect3D();

	/*Init lights. */
	InitLights();
}

void 
DirectX11Renderer::InitLights()
{
	this->m_Light.m_Direction = FLOAT3(0.25f, 0.5f, -1.0f);
	this->m_Light.m_ColourAmbient = FLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	this->m_Light.m_ColourDiffuse = FLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	this->m_Lights.push_back(this->m_Light);
	this->m_LightManager.AddDirectionalLight(&this->m_Light);

	this->m_pFrameUniformBuffer = DirectX11Buffer::CreateConstantBuffer(this->m_pDevice, sizeof(FrameUniformDescriptor), true, false, NULL);
	assert(this->m_pFrameUniformBuffer);
	this->ReloadLightBuffer();
}

void 
DirectX11Renderer::InitEventHandlers()
{
	EventListenerDelegate eDelegate;// MakeDelegate(this, &this->OnResolutionChanged);
	eDelegate = MakeDelegate(this, &DirectX11Renderer::OnResolutionChanged);
	this->m_pEventManager->VAddListener(eDelegate, EventType::EVENT_TYPE_RESOLUTION_CHANGED);
}

void 
DirectX11Renderer::InitDirect3D()
{
	DXGI_SWAP_CHAIN_DESC swapChainDescriptor;
	ZeroMemory(&swapChainDescriptor, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDescriptor.BufferCount = 1;
	swapChainDescriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // We're using standard 32-bit colours for now. Would need to be increased for HDR support.
	swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescriptor.OutputWindow = g_Window; //Well, fuck.
	swapChainDescriptor.SampleDesc.Count = 4; //TODO(Dino): Make configurable.
	swapChainDescriptor.Windowed = true;


	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};


	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&swapChainDescriptor,
		&this->m_pSwapChain,
		&this->m_pDevice,
		&this->m_featureLevel,
		&this->m_pDeviceContext);

	/*Sets the backbuffer to be the current render target. */
	ID3D11Texture2D *pBackBuffer;
	this->m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *) &pBackBuffer);
	this->m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &this->m_pRenderTargetView);
	pBackBuffer->Release();

	this->m_pDeviceContext->OMSetRenderTargets(1, &this->m_pRenderTargetView, NULL);

	/*Sets up the viewport. */
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 2560,
	viewport.Height = 1440,
	viewport.MinDepth = VIEWPORT_DEPTH_MIN;
	viewport.MaxDepth = VIEWPORT_DEPTH_MAX;

	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;
	this->m_pDevice->CreateRasterizerState(&wfdesc, &this->m_pRasterizerStateWireframe);
	this->m_pDeviceContext->RSSetState(NULL);

	this->m_pDeviceContext->RSSetViewports(1, &viewport);
	this->m_ClearColour = { 0.0f, 0.0f, 0.0f, 1.0f };

	InitDepthBuffer();
	InitShader();
	InitVertexBuffer();
	InitTransformationPipeline();
	InitBlendStates();

	InitTextRenderer();
}

void 
DirectX11Renderer::InitTextRenderer()
{
	IDXGISurface * pDxgiSurface;
	IDXGIDevice * pDxgiDevice;

	this->m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDxgiDevice);
	this->m_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), (void **)&pDxgiSurface);

	this->m_pTextRenderer = new DirectX11TextRenderer(pDxgiDevice, pDxgiSurface);
	this->m_pTextRenderer->Init();
	this->m_pTextRenderer->SetText(L"DINO");
}

void 
DirectX11Renderer::InitBlendStates()
{
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	HRESULT errCode;
	errCode = this->m_pDevice->CreateBlendState(&blendDesc, &this->m_pBlendStateTransparency);
	assert(errCode == S_OK);
	D3D11_RASTERIZER_DESC cmdesc;
	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));

	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;

	cmdesc.FrontCounterClockwise = true;
	errCode = this->m_pDevice->CreateRasterizerState(&cmdesc, &this->m_pCounterClockWisecullMode);
	assert(errCode == S_OK);

	cmdesc.FrontCounterClockwise = false;
	errCode = this->m_pDevice->CreateRasterizerState(&cmdesc, &this->m_pClockWiseCullMode);
	assert(errCode == S_OK);

	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_NONE;

	this->m_pDevice->CreateRasterizerState(&rastDesc, &this->m_pDisableCullingMode);
}

void
DirectX11Renderer::ResetBlendState()
{
	this->m_pDeviceContext->OMSetBlendState(0, 0, 0xfffffff);
}

void
DirectX11Renderer::SetBlendStateTransparent()
{
	float blendFactor[] = { 0.75f, 0.75f, 0.75f, 1.0f };
	this->m_pDeviceContext->OMSetBlendState(this->m_pBlendStateTransparency, blendFactor, 0xfffffff);
}

void
DirectX11Renderer::SetRasterizerToWireFrame()
{
	this->m_pDeviceContext->RSSetState(this->m_pRasterizerStateWireframe);
}

CORE_BOOLEAN
DirectX11Renderer::InitDepthBuffer()
{
	CORE_BOOLEAN wasInitted = true;

	HRESULT result = S_OK;

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = 2560;
	depthStencilDesc.Height = 1440;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	result = this->m_pDevice->CreateTexture2D(&depthStencilDesc, NULL, &this->m_pDepthStencilBuffer);
	if (result != S_OK)
	{
		wasInitted = false;
		goto end;
	}

	result = this->m_pDevice->CreateDepthStencilView(this->m_pDepthStencilBuffer, NULL, &this->m_pDepthStencilView);
	if (result != S_OK)
	{
		wasInitted = false;
		goto end;
	}

	this->m_pDeviceContext->OMSetRenderTargets(1, &this->m_pRenderTargetView, this->m_pDepthStencilView);

end:
	return wasInitted;
}

void 
DirectX11Renderer::InitShader()
{
	/*Let's init the rendering pipeline itself. */
	DirectX11Shader * pShader = new DirectX11Shader("shaders.shader", "shaders.shader", 1);

	DirectXShaderBufferDescriptor vertexShaderDescriptor = pShader->GetVertexShaderBufferPointer();
	ID3D11VertexShader const  * pVertexShader = pShader->GetVertexShader();
	this->m_pDevice->CreateVertexShader(vertexShaderDescriptor.m_pBuffer, vertexShaderDescriptor.m_size, NULL, (ID3D11VertexShader **)
		&pVertexShader);
	this->m_pDeviceContext->VSSetShader((ID3D11VertexShader *)pVertexShader, 0, 0);

	DirectXShaderBufferDescriptor pixelShaderDescriptor = pShader->GetPixelShaderBufferPointer();
	ID3D11PixelShader const  * pPixelShader = pShader->GetPixelShader();
	this->m_pDevice->CreatePixelShader(pixelShaderDescriptor.m_pBuffer, pixelShaderDescriptor.m_size, NULL, (ID3D11PixelShader **)
		&pPixelShader);
	this->m_pDeviceContext->PSSetShader((ID3D11PixelShader *)pPixelShader, 0, 0);

	this->m_pActiveShader = pShader;

	this->RegisterShader(pShader);
}

void 
DirectX11Renderer::InitVertexBuffer()
{
	Mesh *  pMesh = new Mesh();
	Vertex * pCurrVertex = NULL;
	
	pCurrVertex = new Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f);
	pMesh->AddVertex(pCurrVertex);
	
	pCurrVertex = new Vertex(-1.0f, +1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, -1.0f);
	pMesh->AddVertex(pCurrVertex);

	pCurrVertex = new Vertex(+1.0f, +1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f);
	pMesh->AddVertex(pCurrVertex);

	pCurrVertex = new Vertex(+1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f);
	pMesh->AddVertex(pCurrVertex);


	/* PART 2. */
	pCurrVertex = new Vertex(-1.0f, -1.0f, +1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f);
	pMesh->AddVertex(pCurrVertex);

	pCurrVertex = new Vertex(1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f);
	pMesh->AddVertex(pCurrVertex);

	pCurrVertex = new Vertex(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	pMesh->AddVertex(pCurrVertex);

	pCurrVertex = new Vertex (-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	pMesh->AddVertex(pCurrVertex);
	
	/* Part 3. */
	pCurrVertex = new Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 1.0f, -1.0f);
	pMesh->AddVertex(pCurrVertex);

	pCurrVertex = new Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	pMesh->AddVertex(pCurrVertex);

	pCurrVertex = new Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	pMesh->AddVertex(pCurrVertex);

	pCurrVertex = new Vertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);
	pMesh->AddVertex(pCurrVertex);

	/* Part 4. */
	pCurrVertex = new Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f);
	pMesh->AddVertex(pCurrVertex);

	pCurrVertex = new Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f);
	pMesh->AddVertex(pCurrVertex);

	pCurrVertex = new Vertex(1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f);
	pMesh->AddVertex(pCurrVertex);

	pCurrVertex = new Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 1.0f);
	pMesh->AddVertex(pCurrVertex);

	/* Part 5. */
	pCurrVertex = new Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 1.0f);
	pMesh->AddVertex(pCurrVertex);

	pCurrVertex = new Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	pMesh->AddVertex(pCurrVertex);

	pCurrVertex = new Vertex(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, -1.0f);
	pMesh->AddVertex(pCurrVertex);

	pCurrVertex = new Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f);
	pMesh->AddVertex(pCurrVertex);


	/* Part 6. */
	pCurrVertex = new Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f);
	pMesh->AddVertex(pCurrVertex);

	pCurrVertex = new Vertex(1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, -1.0f);
	pMesh->AddVertex(pCurrVertex);

	pCurrVertex = new Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	pMesh->AddVertex(pCurrVertex);

	pCurrVertex = new Vertex(1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f);
	pMesh->AddVertex(pCurrVertex);


	pMesh->AddIndice(0, 1, 2);
	pMesh->AddIndice(0, 2, 3);

	pMesh->AddIndice(4, 5, 6);
	pMesh->AddIndice(4, 7, 6);

	pMesh->AddIndice(8, 9, 10);
	pMesh->AddIndice(8, 10, 11);

	pMesh->AddIndice(12, 13, 14);
	pMesh->AddIndice(12, 14, 15);

	pMesh->AddIndice(16, 17, 18);
	pMesh->AddIndice(16, 18, 19);

	pMesh->AddIndice(20, 21, 22);
	pMesh->AddIndice(20, 22, 23);

	DirectX11Texture2D * pTex = DirectX11Texture2D::FromDDSFile(this->m_pDevice, L"Assets\\Textures\\wood.dds", 1);
	this->RegisterTexture(pTex);
	DirectX11Renderable * pRenderable = new DirectX11Renderable(pMesh, pTex, this->m_pActiveShader);
	this->m_pRenderables.push_back(pRenderable);

	D3D11_INPUT_ELEMENT_DESC inputElementDescriptor[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	DirectXShaderBufferDescriptor descriptor = this->m_pActiveShader->GetVertexShaderBufferPointer();
	this->m_pDevice->CreateInputLayout(inputElementDescriptor, ARRAYSIZE(inputElementDescriptor), descriptor.m_pBuffer, descriptor.m_size, &this->m_pInputLayout);
	this->m_pDeviceContext->IASetInputLayout(this->m_pInputLayout);

	DirectX11RenderableInstance * pFirstInstance = pRenderable->Instantiate(1, XMMatrixTranslation(5, 0, 0), NULL);
	XMMATRIX childTransform = XMMatrixRotationX(0.3f) * XMMatrixRotationY(-0.6f) * XMMatrixRotationZ(0.46f) * XMMatrixTranslation(-7.f, 2.f, -3.f);
	pRenderable->Instantiate(1, childTransform, pFirstInstance);
	pRenderable->Buffer(this->m_pDevice, this->m_pDeviceContext);
	pRenderable->ActivateBuffers(this->m_pDeviceContext);

	CORE_BOOLEAN wasInserted = this->m_RenderSet.Insert(pRenderable);
	assert(wasInserted);
}

void DirectX11Renderer::SetTexture(DirectX11Texture2D * pTex)
{
	this->m_pDeviceContext->PSSetShaderResources(0, 1, &pTex->m_pShaderResourceView);
	this->m_pDeviceContext->PSSetSamplers(0, 1, &pTex->m_pSampler);
	
	this->m_pActiveTexture = pTex;
}

CORE_BOOLEAN
DirectX11Renderer::InitTransformationPipeline()
{
	CORE_BOOLEAN wasInitted = true;
	
	this->m_pUniformBuffer = DirectX11Buffer::CreateConstantBuffer(this->m_pDevice, sizeof(InstanceUniformDescriptor), true, false, NULL);
	assert(this->m_pUniformBuffer);

	this->m_CameraPosition = FASTVEC_SET(0.0f, 3.0f, -8.0f, 0.0f);
	this->m_CameraTarget = FASTVEC_SET(0.0f, 0.0f, 0.0f, 0.0f);
	this->m_CameraUp = FASTVEC_SET(0.0f, 1.0f, 0.0, 0.0f);

	this->m_CameraView = XMMatrixLookAtLH(this->m_CameraPosition, this->m_CameraTarget, this->m_CameraUp);
	this->m_CameraProjection = XMMatrixPerspectiveFovLH(0.4f*3.14f, (float)2560 / 1440, VIEW_NEAR, VIEW_FAR);

	/*NOTE(Dino): 'tis only an example world matrix, so the equation pans out. It will need to be modified for every object, of course. */
	this->m_World = FASTMAT_IDENTITY();
	this->m_WorldViewProjection = this->m_World * this->m_CameraView * this->m_CameraProjection;
	this->m_PerObjectBuffer.WorldViewProjection = FASTMAT_TRANSPOSE(this->m_WorldViewProjection);

	{
		/*NOTE(Dino): Sets the uniform buffer. */
		auto var = this->m_pUniformBuffer->GetRawPointer();
		this->m_pDeviceContext->UpdateSubresource(var, 0, NULL, &this->m_FrameUniforms, 0, 0);
		this->m_pDeviceContext->VSSetConstantBuffers(0, 1, &var);
	}

	return wasInitted;
}

void 
DirectX11Renderer::UpdateFrameUniforms()
{
	auto pUniformBuffer = this->m_pFrameUniformBuffer->GetRawPointer();
	this->m_pDeviceContext->UpdateSubresource(pUniformBuffer, 0, NULL, &this->m_FrameUniforms, 0, 0);
	this->m_pDeviceContext->PSSetConstantBuffers(0, 1, &pUniformBuffer);

	//auto pLightsBuffer = this->m_pFrameUniformStructuredBuffer->GetRawPointer();
	//this->m_pDeviceContext->PSSetConstantBuffers(1, 1, &pLightsBuffer);
}


void
DirectX11Renderer::SetShader(const DirectX11Shader * pShader)
{
	assert(pShader);
	if (pShader != this->m_pActiveShader)
	{
		this->m_pActiveShader = (DirectX11Shader *) pShader;
		
		ID3D11VertexShader * pVertexShader = (ID3D11VertexShader *) ((DirectX11Shader *)pShader)->GetVertexShader();
		this->m_pDeviceContext->VSSetShader(pVertexShader, 0, 0);

		ID3D11PixelShader * pPixelShader = (ID3D11PixelShader *) ((DirectX11Shader *)pShader)->GetPixelShader();
		this->m_pDeviceContext->PSSetShader(pPixelShader, 0, 0);
	}
}

void
DirectX11Renderer::ShutDown()
{
	this->m_LightManager.ClearAllLights();
	CloseDirectX11Device();
}

void 
DirectX11Renderer::CloseDirectX11Device()
{
	this->m_pDepthStencilView->Release();
	this->m_pDepthStencilBuffer->Release();
	delete this->m_pUniformBuffer;
	delete this->m_pFrameUniformBuffer;
	this->m_pRasterizerStateWireframe->Release();

	this->m_pSwapChain->Release();

	this->m_pBlendStateTransparency->Release();
	this->m_pCounterClockWisecullMode->Release();
	this->m_pClockWiseCullMode->Release();
	this->m_pDisableCullingMode->Release();

	delete this->m_pTextRenderer;
	
	this->m_pDevice->Release();
	this->m_pDeviceContext->Release();
}

void
DirectX11Renderer::Update(CORE_DOUBLE dT)
{
	RenderAll(dT);
}

void 
DirectX11Renderer::RenderAll(CORE_DOUBLE dT)
{
	size_t numShaderSwitches = 0;
	size_t numTextureSwitches = 0;

	size_t numRenderables = 0;
	size_t numRenderableInstances = 0;

	float backgroundColor[4] = { this->m_ClearColour.x, this->m_ClearColour.y, this->m_ClearColour.z, this->m_ClearColour.w };
	this->m_pDeviceContext->ClearRenderTargetView(this->m_pRenderTargetView, backgroundColor);

	this->m_pDeviceContext->ClearDepthStencilView(this->m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	FASTMAT4 cameraViewProjectionMatrix = this->m_CameraView * this->m_CameraProjection;

	this->ResetBlendState();
	RenderAllInSet(&this->m_RenderSet.m_OpaqueRenderables, numTextureSwitches, numRenderableInstances, numRenderableInstances, cameraViewProjectionMatrix);
	this->SetBlendStateTransparent();

	/*NOTE(Dino): Update per-frame uniform buffer. */
	this->m_FrameUniforms.light = this->m_Light;
	this->UpdateFrameUniforms();

	/*
	 * NOTE(Dino): 
	 * We need to render the transparent renderables twice. 
	 * First, we render the back-size, and then the front-facing side.
	 */
	this->m_pDeviceContext->RSSetState(this->m_pCounterClockWisecullMode);
	RenderAllInSet(&this->m_RenderSet.m_TransparentRenderables, numShaderSwitches, numTextureSwitches, numRenderableInstances, cameraViewProjectionMatrix);
	this->m_pDeviceContext->RSSetState(this->m_pClockWiseCullMode);
	RenderAllInSet(&this->m_RenderSet.m_TransparentRenderables, numShaderSwitches, numTextureSwitches, numRenderableInstances, cameraViewProjectionMatrix);
	this->ResetBlendState();
	
	wstringstream wss;
	wss.precision(15);
	wss << dT;
	const wstring ws(wss.str());
	this->m_pTextRenderer->SetText(ws.c_str());
	this->m_pTextRenderer->Render();
	this->m_pSwapChain->Present(0, 0);
}

void 
DirectX11Renderer::RenderAllInSet(DirectX11RenderableMap * pMap, size_t &numShaderSwitches, size_t &numTextureSwitches, size_t &numRenderableInstances, const FASTMAT4 &cameraViewProjectionMatrix)
{
	auto perShaderIterator = pMap->begin();
	while (perShaderIterator != pMap->end())
	{
		CORE_ID shaderId = perShaderIterator->first;
		assert(shaderId);
		auto shaderIter = this->m_ShaderMap.find(shaderId);
		assert(shaderIter != this->m_ShaderMap.end());
		const DirectX11Shader * pShader = shaderIter->second;
		assert(pShader);
		if (this->m_pActiveShader != pShader)
		{
			++numShaderSwitches;
			this->SetShader(pShader);
		}

		DirectX11RenderablesPerTextureSet * pPerTexSet = &(*perShaderIterator->second);
		auto pPerTexSetIter = pPerTexSet->begin();
		CORE_ID textureId = pPerTexSetIter->first;
		assert(textureId);
		auto textureIter = this->m_TextureMap.find(textureId);
		assert(textureIter != this->m_TextureMap.end());
		const DirectX11Texture2D * pTexture = textureIter->second;
		assert(pTexture);
		if (this->m_pActiveTexture != pTexture)
		{
			++numTextureSwitches;
			this->SetTexture((DirectX11Texture2D *)pTexture);
		}

		while (pPerTexSetIter != pPerTexSet->end())
		{
			DirectX11RenderableList * pRenderables = pPerTexSetIter->second;
			for (size_t i = 0; i < pRenderables->size(); i++)
			{
				DirectX11Renderable * pRenderable = pRenderables->at(i);
				assert(pRenderable);
				pRenderable->ActivateBuffers(this->m_pDeviceContext);

				DirectX11RenderableInstanceIterator instanceIterator = pRenderable->GetInstances();
				while (instanceIterator != pRenderable->GetInstancesEnd())
				{
					++numRenderableInstances;
					DirectX11RenderableInstance * pRenderableInstance = *instanceIterator;
					const FASTMAT4 * const  pInstanceTransform = pRenderableInstance->GetCachedTransform();
					FASTMAT4 modelViewProjectionMatrix = *pInstanceTransform * cameraViewProjectionMatrix;
					this->m_PerObjectBuffer.WorldViewProjection = FASTMAT_TRANSPOSE(modelViewProjectionMatrix);
					this->m_PerObjectBuffer.World = FASTMAT_TRANSPOSE(*pInstanceTransform);
					ID3D11Buffer * pUniformBufferPointer = this->m_pUniformBuffer->GetRawPointer();
					this->m_pDeviceContext->UpdateSubresource(pUniformBufferPointer, 0, NULL, &this->m_PerObjectBuffer, 0, 0);
					this->m_pDeviceContext->VSSetConstantBuffers(0, 1, &pUniformBufferPointer);

					assert(pRenderableInstance);
					instanceIterator++;
					pRenderable->Render(this->m_pDeviceContext);
				}
			}
			pPerTexSetIter++;
		}
		perShaderIterator++;
	}
}

void
DirectX11Renderer::RegisterShader(DirectX11Shader * pShader)
{
	assert(pShader);
	assert(pShader->GetIdentifier());

	CORE_ID shaderId = pShader->GetIdentifier();
	pair<CORE_ID, DirectX11Shader *> pair = make_pair(shaderId, pShader);
	this->m_ShaderMap.emplace(pair);
}

void
DirectX11Renderer::DeRegisterShader(CORE_ID shaderId)
{
	assert(shaderId);
	this->m_ShaderMap.erase(shaderId);
}


void
DirectX11Renderer::RegisterTexture(DirectX11Texture2D * pTexture)
{
	assert(pTexture);
	assert(pTexture->m_Identifier);
	pair<CORE_ID, DirectX11Texture2D *> pair = make_pair(pTexture->m_Identifier, pTexture);
	this->m_TextureMap.emplace(pair);
}

void
DirectX11Renderer::DeRegisterTexture(CORE_ID textureId)
{
	assert(textureId);
	this->m_TextureMap.erase(textureId);
}


void
DirectX11Renderer::ReloadLightBuffer()
{
	DirectionalLight * pAsArray = this->m_Lights.data();
	size_t numLights = this->m_Lights.size();

	D3D11_SUBRESOURCE_DATA resourceData;
	resourceData.SysMemPitch = (numLights * sizeof(DirectionalLight));
	resourceData.SysMemSlicePitch = sizeof(DirectionalLight);
	resourceData.pSysMem = &this->m_Light;

	if (this->m_pFrameUniformStructuredBuffer)
		delete this->m_pFrameUniformStructuredBuffer;
	this->m_pFrameUniformStructuredBuffer = DirectX11Buffer::CreateStructuredBuffer(this->m_pDevice, numLights, sizeof(DirectionalLight), true, false, &resourceData);

	ID3D11Buffer *pBuffers[] = { this->m_pUniformBuffer->GetRawPointer(), this->m_pFrameUniformStructuredBuffer->GetRawPointer() };

	auto ptr = this->m_pFrameUniformStructuredBuffer->GetRawPointer();
	this->m_pDeviceContext->UpdateSubresource(ptr, 0, 0, pAsArray, 0, 0);
	this->m_pDeviceContext->PSSetConstantBuffers(0, ARRAYSIZE(pBuffers), pBuffers);

	//this->UpdateFrameUniforms();
}

DirectX11Renderer::DirectX11Renderer(IEventManager * pManager)
{
	assert(pManager);
	this->m_pEventManager = pManager;
	this->m_pFrameUniformStructuredBuffer = NULL;
}


void
DirectX11Renderer::OnResolutionChanged(IEventData * pEvent)
{
	assert(pEvent);
	ResolutionChangedEventData * pEventData = (ResolutionChangedEventData *)pEvent;
	this->ChangeResolution(pEventData->m_Width, pEventData->m_Height);
}

void
DirectX11Renderer::ChangeResolution(CORE_DWORD width, CORE_DWORD height)
{
	this->m_Width = width;
	this->m_Height = height;
	/*NOTE(Dino): Changing the actual resolution is very, very hard. */
}