#include "DirectX11Renderer.h"
#include <sstream>


#include "FastDelegate.h"
#include "FastDelegateBind.h"
#include "AssetEvents.h"
#include "ResolutionChangedEventData.h"
#include "KeyEvents.h"
#include "EntityEvents.h"
#include "RenderableComponent.h"
#include "TransformComponent.h"
#include "DirectX11BufferSlots.h"
#include "CoreEngine.h"
#include <dxgi.h>

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")

#define VIEWPORT_DEPTH_MIN 0.0f
#define VIEWPORT_DEPTH_MAX 1.0f

#define VIEW_NEAR 1.0f
#define VIEW_FAR 1000.0f

#define RES_WIDTH 2550
#define RES_HEIGHT 1440
extern HWND g_Window;

using namespace fastdelegate;


VEC4 DefaultForward     = VEC4(0.0f, 0.0f, 1.0f, 0.0f);
VEC4 DefaultRight       = VEC4(1.0f, 0.0f, 0.0f, 0.0f);
VEC4 CameraForward      = VEC4(0.0f, 0.0f, 1.0f, 0.0f);
VEC4 CameraRight        = VEC4(1.0f, 0.0f, 0.0f, 0.0f);
VEC4 CameraUp           = VEC4(0.0f, 1.0f, 0.0f, 0.0f);

void
CreateMatFromTransform (TransformComponent * pComponent, MAT4 &mat);

void
DirectX11Renderer::Init()
{
	InitEventHandlers();
	InitDirect3D();
	InitMaterial ();
	//InitTerrain();
	InitSquare();
    InitLights ();
}

void 
DirectX11Renderer::InitLights()
{
	this->m_Light.m_Colour				= FLOAT3(0.0f, 0.0f, 0.1f);
	this->m_Light.m_Direction           = FLOAT3(1.0f, 1.0, 1.0f);
	this->m_Light.m_Pad					= FLOAT2(0, 0);

	this->m_Lights.push_back(this->m_Light);
	this->m_LightManager.AddDirectionalLight(&this->m_Light);

	//this->ReloadLightBuffer();
}

void DirectX11Renderer::InitUniformBuffers ()
{
    this->m_pFrameUniformBuffer = DirectX11Buffer::CreateConstantBuffer (this->m_pDevice, FrameUniformDescriptor::GetSize(), true, false, NULL);
    assert (this->m_pFrameUniformBuffer);

	this->m_pObjectUniformBuffer = DirectX11Buffer::CreateConstantBuffer(this->m_pDevice, sizeof(InstanceUniformDescriptor), true, false, NULL);
	assert(this->m_pObjectUniformBuffer);

	this->m_pLightUniformBuffer = DirectX11Buffer::CreateConstantBuffer (this->m_pDevice, sizeof (m_LightUniformDescriptor), true, false, NULL);
	assert (this->m_pLightUniformBuffer);
}

void 
DirectX11Renderer::InitEventHandlers()
{
	EventListenerDelegate eDelegate;
	eDelegate                               = MakeDelegate(this, &DirectX11Renderer::OnResolutionChanged);
	this->m_pEventManager->VAddListener(eDelegate, EventType::EVENT_TYPE_RESOLUTION_CHANGED);

	EventListenerDelegate keyDownDelegate   = MakeDelegate(this, &DirectX11Renderer::OnKeyDown);
	this->m_pEventManager->VAddListener(keyDownDelegate, EventType::EVENT_TYPE_KEYDOWN);

    EventListenerDelegate assetLoadedDelegate = MakeDelegate (this, &DirectX11Renderer::OnAssetLoaded);
    this->m_pEventManager->VAddListener (assetLoadedDelegate, EventType::EVENT_TYPE_ASSET_LOADED);

	EventListenerDelegate entityRegisteredDelegate = MakeDelegate(this, &DirectX11Renderer::OnEntityRegistered);
	this->m_pEventManager->VAddListener(entityRegisteredDelegate, EventType::EVENT_TYPE_ENTITY_REGISTERED);

	EventListenerDelegate entityComponentRegisteredDelegate = MakeDelegate (this, &DirectX11Renderer::OnEntityComponentRegistered);
	this->m_pEventManager->VAddListener (entityComponentRegisteredDelegate, EventType::EVENT_TYPE_ENTITY_COMPONENT_ADDED);

	EventListenerDelegate entityComponentDeRegisteredDelegate = MakeDelegate (this, &DirectX11Renderer::OnEntityComponentDeRegistered);
	this->m_pEventManager->VAddListener (entityComponentDeRegisteredDelegate, EventType::EVENT_TYPE_ENTITY_COMPONENT_REMOVED);

	EventListenerDelegate entityComponentChangedDelegate = MakeDelegate (this, &DirectX11Renderer::OnEntityComponentChanged);
	this->m_pEventManager->VAddListener (entityComponentChangedDelegate, EventType::EVENT_TYPE_ENTITY_COMPONENT_CHANGED);
}

CORE_BOOLEAN
DirectX11Renderer::InitRenderTarget ()
{
	CORE_BOOLEAN isSuccess			= FALSE;
	HRESULT result;

	this->m_RenderTarget = DirectX11Texture2D::AsRenderTarget (this->m_pDevice, RES_WIDTH, RES_HEIGHT, 1, 4);
	assert (m_RenderTarget);
	if (m_RenderTarget)
	{
		this->m_TextureMap[0] = this->m_RenderTarget;

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format				= DXGI_FORMAT_R32G32B32A32_FLOAT; //TODO(Dino): Configurable.
		renderTargetViewDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2DMS;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		
		auto pTex								= this->m_RenderTarget->GetRawPointer ();
		result									= this->m_pDevice->CreateRenderTargetView (pTex, &renderTargetViewDesc, &m_pRenderTargetViewTex);
		assert (result == S_OK);
		if (result == S_OK)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
			ZeroMemory (&shaderResourceViewDesc, sizeof (shaderResourceViewDesc));
			shaderResourceViewDesc.Format			= DXGI_FORMAT_R32G32B32A32_FLOAT;
			shaderResourceViewDesc.ViewDimension	= D3D11_SRV_DIMENSION_TEXTURE2DMS;
			shaderResourceViewDesc.Texture2D.MipLevels			= 1;
			shaderResourceViewDesc.Texture2D.MostDetailedMip	= 0;

			result = this->m_pDevice->CreateShaderResourceView (pTex, &shaderResourceViewDesc, &this->m_pShaderResourceView);
			assert (result == S_OK);
			isSuccess = (result == S_OK);
		}
	}
	return isSuccess;
}

void 
DirectX11Renderer::InitDirect3D()
{
	m_MsaaX = 4;
	m_MsaaQ = 0;
#pragma region Initialize swap chain.
	DXGI_SWAP_CHAIN_DESC swapChainDescriptor;
	ZeroMemory (&swapChainDescriptor, sizeof (DXGI_SWAP_CHAIN_DESC));

	swapChainDescriptor.Windowed			= 1;
	swapChainDescriptor.BufferCount			= 2;
	swapChainDescriptor.BufferDesc.Format	= m_DisplayFormat;
	swapChainDescriptor.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescriptor.SampleDesc.Count	= m_MsaaX;
	swapChainDescriptor.SampleDesc.Quality	= m_MsaaQ;
	swapChainDescriptor.OutputWindow		= g_Window;
	swapChainDescriptor.BufferDesc.Height	= RES_HEIGHT;
	swapChainDescriptor.BufferDesc.Width	= RES_WIDTH;
	swapChainDescriptor.SwapEffect			= DXGI_SWAP_EFFECT_SEQUENTIAL;
	
	auto hr = S_OK;

	ComPtr<IDXGIFactory1> pFactory = NULL;
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

	 hr = D3D11CreateDeviceAndSwapChain (NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_SINGLETHREADED,
		featureLevels,
		ARRAYSIZE (featureLevels),
		D3D11_SDK_VERSION,
		&swapChainDescriptor,
		&this->m_pSwapChain,
		&this->m_pDevice,
		&this->m_featureLevel,
		&this->m_pDeviceContext);
	assert (hr == S_OK);

#pragma endregion
#pragma region Initialize back-buffer and render target.
	///*Sets the backbuffer to be the current render target. */
	ID3D11Texture2D *pBackBuffer;

	this->m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *) &pBackBuffer);
	this->m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &this->m_pRenderTargetViewBackBuffer);
	pBackBuffer->Release();
	this->m_pDeviceContext->OMSetRenderTargets(1, &this->m_pRenderTargetViewBackBuffer, NULL);

#pragma endregion
#pragma region Initialize viewport.
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX   = 0;
	viewport.TopLeftY   = 0;
	viewport.Width      = RES_WIDTH,
	viewport.Height     = RES_HEIGHT,
	viewport.MinDepth   = VIEWPORT_DEPTH_MIN;
	viewport.MaxDepth   = VIEWPORT_DEPTH_MAX;
	this->m_pDeviceContext->RSSetViewports(1, &viewport);
#pragma endregion
#pragma region Initialize rasterizer states.
	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;
	wfdesc.MultisampleEnable = 1;
	wfdesc.DepthClipEnable = 1;
	this->m_pDevice->CreateRasterizerState(&wfdesc, &this->m_pRasterizerStateWireframe);
	this->m_pDeviceContext->RSSetState(NULL);
#pragma endregion

	this->m_ClearColour = { 0.2f, 0.2f, 0.2f, 1.0f };

	InitDepthBuffer();
	this->InitRenderTarget ();
	InitShaders();
	InitTextures();
	InitCamera();
	InitBlendStates();
	InitTextRenderer();
    InitUniformBuffers ();
}

void
DirectX11Renderer::SetRenderTargetBackBuffer()
{
	this->m_pDeviceContext->OMSetRenderTargets(1, &this->m_pRenderTargetViewBackBuffer, NULL);

}

void
DirectX11Renderer::SetRenderTargetTexture()
{
	this->m_pDeviceContext->OMSetRenderTargets(1, &this->m_pRenderTargetViewTex, NULL);
}

void
DirectX11Renderer::ClearRenderTargetTex ()
{
	float backgroundColor[4] = { this->m_ClearColour.x, this->m_ClearColour.y, this->m_ClearColour.z, this->m_ClearColour.w };
	this->m_pDeviceContext->ClearRenderTargetView (this->m_pRenderTargetViewTex, backgroundColor);

	//if (this->m_pDepthStencilViewTex)
	//	this->m_pDeviceContext->ClearDepthStencilView (this->m_pDepthStencilViewTex, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void
DirectX11Renderer::ClearRenderTargetBackBuffer ()
{
	float backgroundColor[4] = { this->m_ClearColour.x, this->m_ClearColour.y, this->m_ClearColour.z, this->m_ClearColour.w };
	this->m_pDeviceContext->ClearRenderTargetView (this->m_pRenderTargetViewBackBuffer, backgroundColor);
	this->m_pDeviceContext->ClearDepthStencilView (this->m_pDepthStencilViewBackBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
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
	depthStencilDesc.Width = RES_WIDTH;
	depthStencilDesc.Height = RES_HEIGHT;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = m_MsaaX;
	depthStencilDesc.SampleDesc.Quality = m_MsaaQ;
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

	result = this->m_pDevice->CreateTexture2D (&depthStencilDesc, NULL, &this->m_pDepthStencilTex);
	if (result != S_OK)
	{
		wasInitted = false;
		goto end;
	}


	result = this->m_pDevice->CreateDepthStencilView(this->m_pDepthStencilBuffer, NULL, &this->m_pDepthStencilViewBackBuffer);
	if (result != S_OK)
	{
		wasInitted = false;
		goto end;
	}

	result = this->m_pDevice->CreateDepthStencilView (this->m_pDepthStencilTex, NULL, &this->m_pDepthStencilViewBackBuffer);
	if (result != S_OK)
	{
		wasInitted = false;
		goto end;
	}

	this->m_pDeviceContext->OMSetRenderTargets(1, &this->m_pRenderTargetViewBackBuffer, NULL);

end:
	assert (result == S_OK);
	return wasInitted;
}

void 
DirectX11Renderer::InitShaders()
{
#pragma region Init red shader
	{
		this->m_pActiveShader = NULL;

		DirectX11Shader * pShader = new DirectX11Shader(L"Assets\\Shaders\\red.hlsl", "VShader", L"Assets\\Shaders\\red.hlsl", "PShader", 1);

		ID3D11VertexShader ** pVertexShader = pShader->GetVertexShader();
		DirectXShaderBufferDescriptor vertexShaderDescriptor = pShader->GetVertexShaderBufferPointer();
		HRESULT errVsh = this->m_pDevice->CreateVertexShader(vertexShaderDescriptor.m_pBuffer, vertexShaderDescriptor.m_size, NULL, pVertexShader);
		this->m_pDeviceContext->VSSetShader(*pVertexShader, 0, 0);

		ID3D11PixelShader ** pPixelShader = pShader->GetPixelShader();
		DirectXShaderBufferDescriptor pixelShaderDescriptor = pShader->GetPixelShaderBufferPointer();
		HRESULT errPsh = this->m_pDevice->CreatePixelShader(pixelShaderDescriptor.m_pBuffer, pixelShaderDescriptor.m_size, NULL, pPixelShader);
		this->m_pDeviceContext->PSSetShader(*pPixelShader, 0, 0);


		DirectXShaderBufferDescriptor descriptor = pShader->GetVertexShaderBufferPointer();
		ID3D11InputLayout * pLayout = NULL;
		HRESULT res = this->m_pDevice->CreateInputLayout(this->m_inputElementDescriptor, ARRAYSIZE(m_inputElementDescriptor), descriptor.m_pBuffer, descriptor.m_size, &pLayout);
		pShader->SetInputLayout(pLayout);
		this->m_pDeviceContext->IASetInputLayout(pLayout);

		this->RegisterShader(pShader);
		this->m_pActiveShader = pShader;
		this->m_pRedShader = pShader;

		ShaderDescriptor * pTexturingShader = new ShaderDescriptor(13, "VShader", L"Assets\\Shaders\\shaders.hlsl", "PShader", L"Assets\\Shaders\\shaders.hlsl");
		DirectX11Shader * pTexShader = this->CreateShader(pTexturingShader);
		this->m_pBasicShader = pTexShader;
		this->RegisterShader(pTexShader);
	}
#pragma endregion

#pragma region init square shader
{
		ShaderDescriptor * pTexturingShader = new ShaderDescriptor(14, "VShader", L"Assets\\Shaders\\square.hlsl", "PShader", L"Assets\\Shaders\\square.hlsl");
		DirectX11Shader * pTexShader = this->CreateShader(pTexturingShader);
		this->RegisterShader(pTexShader);
		this->m_pSquareShader = pTexShader;
}
#pragma endregion
	
}

DirectX11Shader * 
DirectX11Renderer::CreateShader(ShaderDescriptor * pDescriptor)
{
	assert(pDescriptor);

	DirectX11Shader * pRetVal = new DirectX11Shader(pDescriptor->m_VertexShaderPath, pDescriptor->m_VertexShaderEntryPoint, pDescriptor->m_PixelShaderPath, pDescriptor->m_PixelShaderEntryPoint, pDescriptor->m_Identifier);
	
	DirectXShaderBufferDescriptor vertexShaderDescriptor = pRetVal->GetVertexShaderBufferPointer();
	ID3D11VertexShader ** pVertexShader = pRetVal->GetVertexShader();
	HRESULT resVs = this->m_pDevice->CreateVertexShader(vertexShaderDescriptor.m_pBuffer, vertexShaderDescriptor.m_size, NULL, pVertexShader);
	assert(resVs == S_OK);

	DirectXShaderBufferDescriptor pixelShaderDescriptor = pRetVal->GetPixelShaderBufferPointer();
	ID3D11PixelShader ** pPixelShader = pRetVal->GetPixelShader();
	this->m_pDevice->CreatePixelShader(pixelShaderDescriptor.m_pBuffer, pixelShaderDescriptor.m_size, NULL, pPixelShader);

	DirectXShaderBufferDescriptor descriptor = pRetVal->GetVertexShaderBufferPointer();
	ID3D11InputLayout * pLayout = NULL;
	HRESULT res = this->m_pDevice->CreateInputLayout(this->m_inputElementDescriptor, ARRAYSIZE(m_inputElementDescriptor), descriptor.m_pBuffer, descriptor.m_size, &pLayout);
	pRetVal->SetInputLayout(pLayout);

	this->RegisterShader(pRetVal);
	return pRetVal;
}

void
DirectX11Renderer::InitSquare()
{
	Mesh * pSquareMesh		= new Mesh(6);
	pSquareMesh->AddVertex(new Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f));
	pSquareMesh->AddVertex(new Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, -1.0f));
	pSquareMesh->AddVertex(new Vertex(1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f));
	pSquareMesh->AddVertex(new Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f));

	pSquareMesh->AddIndice(0, 1, 2);
	pSquareMesh->AddIndice(0, 2, 3);

	auto pTexture = this->m_pGrassTex;
	auto pShader = this->m_pSquareShader;

	this->m_RenderTarget;
	auto pRenderable = new DirectX11Renderable(pSquareMesh, pTexture, pShader, FALSE);
	auto transform = XMMatrixScaling(4.7f, 4.f, 1.0f) * XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	auto pInstance = pRenderable->Instantiate(1, transform, m_pDefaultMaterial);
	//this->m_RenderSet.Insert (pRenderable);

	this->m_Renderables.push_back(pRenderable);
	pRenderable->Buffer(this->m_pDevice, this->m_pDeviceContext);
}

void 
DirectX11Renderer::InitTerrain()
{
	ULONG terrainWidth = 100;
	ULONG terrainHeight = 100;

	unsigned long index = 0, i, j;
	CORE_REAL positionX, positionZ;
	unsigned long vertexCount = (terrainWidth - 1) * (terrainHeight - 1) * 8;
	unsigned long indexCount = vertexCount;

	Mesh * pDynamicTerrainMesh = new Mesh(5);
	Vertex ** pVertices = new Vertex *[vertexCount];
	unsigned long * pIndices = new unsigned long[indexCount];

	VEC2 colour(1.0f, 1.0f);
	VEC3 normal(0.0f, 0.0f, 0.0f);

	for (j = 0; j < (terrainHeight - 1); j++)
	{
		for (i = 0; i < (terrainWidth - 1); i++)
		{
			/* We generate 4 lines per grid element. */
				
			/* Line 1 - upper left.*/
			positionX = (float)i;
			positionZ = (float) (j + 1);

			VEC3 position(positionX, 0.0f, positionZ);
			pVertices[index] = new Vertex(position, colour, normal);
			pIndices[index] = index;
			index++;

			/* Line 1 - upper right. */
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			VEC3 position2(positionX, 0.0f, positionZ);
			pVertices[index] = new Vertex(position2, colour, normal);
			pIndices[index] = index;
			index++;

			/* Line 2 - upper right. */
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			VEC3 position3(positionX, 0.0f, positionZ);
			pVertices[index] = new Vertex(position3, colour, normal);
			pIndices[index] = index;
			index++;

			/* Line 2 - bottom right. */
			positionX = (float)(i + 1);
			positionZ = (float)j;

			VEC3 position4(positionX, 0.0f, positionZ);
			pVertices[index] = new Vertex(position4, colour, normal);
			pIndices[index] = index;
			index++;

			/* Line 3 - bottom right. */
			positionX = (float)(i + 1);
			positionZ = (float)j;

			VEC3 position5(positionX, 0.0f, positionZ);
			pVertices[index] = new Vertex(position5, colour, normal);
			pIndices[index] = index;
			index++;

			/* Line 3 - bottom left. */
			positionX = (float)i;
			positionZ = (float)j;
				
			VEC3 position6(positionX, 0.0f, positionZ);
			pVertices[index] = new Vertex(position6, colour, normal);
			pIndices[index] = index;
			index++;

			/* Line 4 - bottom left. */
			positionX = (float)i;
			positionZ = (float)j;

			VEC3 position7(positionX, 0.0f, positionZ);
			pVertices[index] = new Vertex(position7, colour, normal);
			pIndices[index] = index;
			index++;

			/* Line 4 - upper left. */
			positionX = (float)i;
			positionZ = (float)(j + 1);

			VEC3 position8(positionX, 0.0f, positionZ);
			pVertices[index] = new Vertex(position8, colour, normal);
			pIndices[index] = index;
			index++;
		}
	}


	for (size_t currIdx = 0; currIdx < vertexCount; currIdx++)
		pDynamicTerrainMesh->AddVertex(pVertices[currIdx]);

	for (size_t currIdx = 0; currIdx < indexCount; currIdx++)
		pDynamicTerrainMesh->AddIndice(pIndices[currIdx]);

	DirectX11Texture2D * pTexture   = this->m_TextureMap.at(1);
	DirectX11Shader * pShader       = this->m_ShaderMap.at(1);

	DirectX11Renderable * pDynamicTerrainRenderable = new DirectX11Renderable(pDynamicTerrainMesh, pTexture, pShader, FALSE, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	this->m_Renderables.push_back(pDynamicTerrainRenderable);

	DirectX11RenderableInstance * pDynamicTerrainInstance = pDynamicTerrainRenderable->Instantiate(5, XMMatrixScaling(10.0f, 10.0f, 10.0f), this->m_pDefaultMaterial, NULL);
	assert(pDynamicTerrainInstance);
 	pDynamicTerrainRenderable->Buffer(this->m_pDevice, this->m_pDeviceContext);
	this->m_RenderSet.Insert(pDynamicTerrainRenderable);
}


void 
DirectX11Renderer::InitTextures()
{
	DirectX11Texture2D * pTex = DirectX11Texture2D::FromDDSFile(this->m_pDevice, L"Assets\\Textures\\wood.dds", 1);
	this->RegisterTexture(pTex);
	this->m_pWoodTex = pTex;

	DirectX11Texture2D  * pGrassTex = DirectX11Texture2D::FromDDSFile(this->m_pDevice,  L"Assets\\Textures\\grass.dds", 2);
	this->RegisterTexture(pGrassTex);
	this->m_pGrassTex = pGrassTex;
}

void
DirectX11Renderer::InitMaterial ()
{
	this->m_pDefaultMaterial = new Material ("__DEFAULT__", 0);
	this->m_pDefaultMaterial->SetAmbient (0, 0, 0, 1);
	this->m_pDefaultMaterial->SetDiffuse (0, 0, 0, 1);
	this->m_pDefaultMaterial->SetSpecular (0, 0, 0, 1);
	this->m_pDefaultMaterial->SetSpecularPower (0);
}

void DirectX11Renderer::SetTexture(DirectX11Texture2D * pTex)
{

	this->m_pDeviceContext->PSSetShaderResources(0, 1, &pTex->m_pShaderResourceView);
	this->m_pDeviceContext->PSSetSamplers(0, 1, &pTex->m_pSampler);
	
	this->m_pActiveTexture = pTex;
}

CORE_BOOLEAN
DirectX11Renderer::InitCamera()
{
	CORE_BOOLEAN wasInitted = true;

	this->m_CameraYaw		= 0;
	this->m_CameraPitch		= 0;
	this->m_MoveLeftRight	= 0.0f;
	this->m_MoveBackForward = 0.0f;

	this->m_CameraPosition	=  VEC4(0.0f, 0.0f, -3.0f, 0.0f);
	this->m_CameraTarget	= VEC4(0.0f, 0.0f, 0.0f, 0.0f);
	this->m_CameraUp		= VEC4(0.0f, 1.0f, 0.0, 0.0f);
	this->m_CameraForward	= VEC4(CameraForward);
	this->m_CameraRight		= CameraRight;

	auto cameraPosition		= XMLoadFloat4(&this->m_CameraPosition);
	auto cameraTarget		= XMLoadFloat4(&this->m_CameraTarget);
	auto cameraUp			= XMLoadFloat4(&this->m_CameraUp);
	
	auto cameraView			= XMMatrixLookAtLH(cameraPosition, cameraTarget, cameraUp);
	XMStoreFloat4x4(&this->m_CameraView, cameraView);
	
	auto cameraProjection	= XMMatrixPerspectiveFovLH(0.4f*3.14f, (float)RES_WIDTH / RES_HEIGHT, VIEW_NEAR, VIEW_FAR);
	XMStoreFloat4x4(&this->m_CameraProjection, cameraProjection);

	return wasInitted;
}

void
DirectX11Renderer::UpdateCamera()
{
	auto cameraRotation		= XMMatrixRotationRollPitchYaw(this->m_CameraPitch, this->m_CameraYaw, 0);;
	XMStoreFloat4x4(&this->m_CameraRotation, cameraRotation);

	auto defaultForward		= XMLoadFloat4(&DefaultForward);
	auto defaultRight		= XMLoadFloat4(&DefaultRight);
	auto cameraUp			= XMLoadFloat4(&CameraUp);
	
	auto cameraTarget		= XMVector3TransformCoord(defaultForward, cameraRotation);
	cameraTarget			= XMVector3Normalize(cameraTarget);
	XMStoreFloat4(&this->m_CameraTarget, cameraTarget);

	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix       = XMMatrixRotationY(this->m_CameraYaw);

	auto cameraRight		= XMVector3TransformCoord(defaultRight, RotateYTempMatrix);
	XMStoreFloat4(&this->m_CameraRight, cameraRight);
	
	cameraUp			= XMVector3TransformCoord(cameraUp, RotateYTempMatrix);
	XMStoreFloat4(&this->m_CameraUp, cameraUp);

	auto cameraForward		= XMVector3TransformCoord(defaultForward, RotateYTempMatrix);
	XMStoreFloat4(&this->m_CameraForward, cameraForward);

	auto cameraPosition		= XMLoadFloat4(&this->m_CameraPosition);
	cameraPosition			+= this->m_MoveLeftRight * cameraRight;
	cameraPosition			+= this->m_MoveBackForward * cameraForward;
	XMStoreFloat4(&this->m_CameraPosition, cameraPosition);

	this->m_MoveLeftRight   = 0;
	this->m_MoveBackForward = 0;

	cameraTarget = cameraPosition + cameraTarget;
	XMStoreFloat4(&this->m_CameraTarget, cameraTarget);
}

void 
DirectX11Renderer::UpdateFrameUniforms()
{
	{
		auto pUniformBuffer     = this->m_pFrameUniformBuffer->GetRawPointer();
		this->m_pDeviceContext->UpdateSubresource(pUniformBuffer, 0, NULL, &this->m_FrameUniforms, 0, 0);
		this->m_pDeviceContext->VSSetConstantBuffers(VERT_SHADER_SLOT_FRAME_MATRICES, 1, &pUniformBuffer);
	}

	{
		auto pUniformBuffer = this->m_pLightUniformBuffer->GetRawPointer ();
		m_pDeviceContext->UpdateSubresource (pUniformBuffer, 0, NULL, &this->m_LightUniformDescriptor, 0, 0);
		m_pDeviceContext->PSSetConstantBuffers (PIX_SHADER_SLOT_LIGHT_UNIFORM, 1, &pUniformBuffer);
	}

}

void
DirectX11Renderer::UpdateInstanceUniforms()
{
	auto pUniformBuffer = this->m_pObjectUniformBuffer->GetRawPointer();
	this->m_pDeviceContext->UpdateSubresource(pUniformBuffer, 0, NULL, &this->m_ObjectUniforms, 0, 0);
	this->m_pDeviceContext->VSSetConstantBuffers(VERT_SHADER_SLOT_INSTANCE_MATRICES, 1, &pUniformBuffer);
}

void
DirectX11Renderer::UpdateUniforms()
{
	this->UpdateFrameUniforms();
	this->UpdateInstanceUniforms();
}


void
DirectX11Renderer::SetShader(DirectX11Shader * pShader)
{
	assert(pShader);
	this->m_pActiveShader                   = pShader;
	
	ID3D11VertexShader ** pVertexShader     = pShader->GetVertexShader();
	ID3D11PixelShader ** pPixelShader       = pShader->GetPixelShader();

	this->m_pDeviceContext->VSSetShader(*pVertexShader, 0, 0);
	this->m_pDeviceContext->PSSetShader(*pPixelShader, 0, 0);
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
	for (auto renderable : this->m_Renderables)
		delete renderable;
	this->m_Renderables.clear ();

	for (auto material : this->m_MaterialMap)
		delete material.second;
	this->m_MaterialMap.clear ();
	
	delete this->m_pGrassTex;
	delete this->m_pWoodTex;

	this->m_pDepthStencilViewTex->Release();
	this->m_pDepthStencilBuffer->Release();
	delete this->m_pObjectUniformBuffer;
	delete this->m_pFrameUniformBuffer;
	delete this->m_pLightUniformBuffer;
	this->m_pRasterizerStateWireframe->Release();
	delete this->m_RenderTarget;
	this->m_pSwapChain->Release();
	this->m_pShaderResourceView->Release ();
	this->m_pBlendStateTransparency->Release();
	this->m_pCounterClockWisecullMode->Release();
	this->m_pClockWiseCullMode->Release();
	this->m_pDisableCullingMode->Release();
    this->m_pFrameUniformBuffer->Release ();;

	delete this->m_pTextRenderer;
	
	this->m_pDevice->Release();
	this->m_pDeviceContext->Release();
}

void
DirectX11Renderer::Update(CORE_DOUBLE dT)
{
	this->UpdateCamera();
	RenderAll(dT);
}

void
DirectX11Renderer::RenderAllSimple(CORE_DOUBLE dT)
{
	this->ClearRenderTargetTex (); //Enable when rendering to texture.
	this->ClearRenderTargetBackBuffer ();

	FASTMAT4 cameraView = XMLoadFloat4x4(&this->m_CameraView);
	FASTMAT4 cameraProjection = XMLoadFloat4x4(&this->m_CameraProjection);
	FASTMAT4 wp = cameraView * cameraProjection;
	MAT4 cameraViewProjectionMatrix;
	XMStoreFloat4x4(&cameraViewProjectionMatrix, wp);

	/*NOTE(Dino): Update per-frame uniform buffer. */
	this->m_LightUniformDescriptor.light = this->m_Light;
	this->UpdateUniforms();

	this->ResetBlendState();

	for (auto renderableIterator = this->m_Renderables.begin(); renderableIterator != this->m_Renderables.end(); renderableIterator++)
	{
		DirectX11Renderable * pRenderable = *renderableIterator;
		assert(pRenderable);

		DirectX11Shader * pShader = (DirectX11Shader *) pRenderable->GetShader();
		this->SetShader(pShader);
		for (auto instanceIterator = pRenderable->GetInstances(); instanceIterator != pRenderable->GetInstancesEnd(); instanceIterator++)
		{
			DirectX11RenderableInstance * pInstance = *instanceIterator;
			auto transform = pInstance->GetCachedTransform();
		}
	}
	this->SetBlendStateTransparent();
}

void 
DirectX11Renderer::RenderAll(CORE_DOUBLE dT)
{
	size_t numShaderSwitches = 0;
	size_t numTextureSwitches = 0;

	size_t numRenderables = 0;
	size_t numRenderableInstances = 0;

	ClearRenderTargetTex();
	this->ClearRenderTargetBackBuffer();

	//this->SetRenderTargetTexture();
	auto cameraView						= XMLoadFloat4x4(&this->m_CameraView);
	auto cameraProjection				= XMLoadFloat4x4(&this->m_CameraProjection);

	FASTMAT4 cameraViewProjectionMatrix = XMMatrixTranspose(cameraView * cameraProjection);
	MAT4 viewProjectionMatrix;
	XMStoreFloat4x4(&viewProjectionMatrix, cameraViewProjectionMatrix);

	/*NOTE(Dino): Update per-frame uniform buffer. */
	this->m_LightUniformDescriptor.light = this->m_Light;
	this->UpdateFrameUniforms ();

	this->ResetBlendState();
	RenderAllInSet(&this->m_RenderSet.m_OpaqueRenderables, numTextureSwitches, numRenderableInstances, numRenderableInstances, viewProjectionMatrix);
	this->SetBlendStateTransparent();


	/*
	 * NOTE(Dino): 
	 * We need to render the transparent renderables twice. 
	 * First, we render the back-size, and then the front-facing side.
	 */
	this->m_pDeviceContext->RSSetState(this->m_pCounterClockWisecullMode);
	RenderAllInSet(&this->m_RenderSet.m_TransparentRenderables, numShaderSwitches, numTextureSwitches, numRenderableInstances, viewProjectionMatrix);
	this->m_pDeviceContext->RSSetState(this->m_pClockWiseCullMode);
	RenderAllInSet(&this->m_RenderSet.m_TransparentRenderables, numShaderSwitches, numTextureSwitches, numRenderableInstances, viewProjectionMatrix);
	this->ResetBlendState();
	
	wstringstream wss;
	wss.precision(15);
	wss << dT;
	const wstring ws(wss.str());
	this->m_pTextRenderer->SetText(ws.c_str());
	this->m_pTextRenderer->Render();

	//SetRenderTargetBackBuffer ();
	this->m_pSwapChain->Present(0, 0);

}

void 
DirectX11Renderer::RenderAllInSet(DirectX11RenderableMap * pMap, size_t &numShaderSwitches, size_t &numTextureSwitches, size_t &numRenderableInstances, const MAT4 &cameraViewProjectionMatrix)
{
	auto perShaderIterator = pMap->begin();
	while (perShaderIterator != pMap->end())
	{
		CORE_ID shaderId = perShaderIterator->first;
		assert(shaderId);
		auto shaderIter = this->m_ShaderMap.find(shaderId);
		assert(shaderIter != this->m_ShaderMap.end());
		DirectX11Shader * pShader = shaderIter->second;
		assert(pShader);
		if (this->m_pActiveShader != pShader)
		{
			ID3D11InputLayout * pLayout = pShader->GetInputLayout();
			this->m_pInputLayout = pLayout;
			this->m_pDeviceContext->IASetInputLayout(pLayout);
			ID3D11VertexShader ** pVertexShader = pShader->GetVertexShader();
			ID3D11PixelShader ** pPixelShader = pShader->GetPixelShader();

			this->m_pDeviceContext->VSSetShader(*pVertexShader, 0, 0);
			this->m_pDeviceContext->PSSetShader(*pPixelShader, 0, 0);

			++numShaderSwitches;
			this->m_pActiveShader = pShader;
		}

		auto pPerTexSet = &(*perShaderIterator->second);
		auto pPerTexSetIter = pPerTexSet->begin();
		if (pPerTexSetIter != pPerTexSet->end ())
		{
			CORE_ID textureId = pPerTexSetIter->first;
			assert(textureId);
			auto textureIter = this->m_TextureMap.find(textureId);
			assert(textureIter != this->m_TextureMap.end());
			const auto pTexture = textureIter->second;
			assert(pTexture);
			if (this->m_pActiveTexture != pTexture)
			{
				++numTextureSwitches;
				this->SetTexture((DirectX11Texture2D *)pTexture);
			}

			while (pPerTexSetIter != pPerTexSet->end())
			{
				auto pRenderables = pPerTexSetIter->second;
				for (size_t i = 0; i < pRenderables->size(); i++)
				{
					auto pRenderable = pRenderables->at(i);
					assert(pRenderable);
					if (pRenderable->GetInstanceCount() == 0)
						continue;
					pRenderable->ActivateBuffers(this->m_pDeviceContext);
#pragma region Segment 1: Instanced rendering.
					if (this->m_pActiveShader == this->m_pRedShader)
					{
						int x = 5;
						x++;
					}
					auto instanceIterator = pRenderable->GetInstances ();
					while (instanceIterator != pRenderable->GetInstancesEnd ())
					{
						// Fill up the instance buffer.

						auto pRenderableInstance		= *instanceIterator;
						auto  pInstanceTransform		= pRenderableInstance->GetCachedTransform ();

						auto instanceTransform			= XMLoadFloat4x4(pInstanceTransform);
						auto cameraTransform			= XMLoadFloat4x4(&this->m_CameraView);
						auto cwp						= XMLoadFloat4x4(&cameraViewProjectionMatrix);


						auto modelViewProjectionMatrix	= instanceTransform * cameraTransform * cwp;
						
						auto wvp						= (modelViewProjectionMatrix);
						auto itt						= (instanceTransform);
						XMStoreFloat4x4(&this->m_ObjectUniforms.WorldViewProjection, wvp);
						XMStoreFloat4x4(&this->m_ObjectUniforms.World, itt);

						this->UpdateInstanceUniforms ();
						instanceIterator++;
						//pRenderable->Render (this->m_pDeviceContext); // Comment out for instanced rendering.

					}
					auto cameraTransform						= XMLoadFloat4x4(&this->m_CameraView);
					auto cwp									= XMLoadFloat4x4(&cameraViewProjectionMatrix);
					auto vpm									= XMMatrixTranspose(cameraTransform * cwp);
					XMStoreFloat4x4(&this->m_FrameUniforms.ViewProjectionMatrix, vpm);
					
					cwp = XMLoadFloat4x4 (&cameraViewProjectionMatrix);
					auto mat = cameraTransform * cwp;
					//XMStoreFloat4x4 (&this->m_FrameUniforms.CameraViewProjectionMatrix, mat);

					this->m_FrameUniforms.Projection			= this->m_CameraProjection;
					this->m_FrameUniforms.Camera				= this->m_CameraView;
					UpdateFrameUniforms ();
					pRenderable->Render (this->m_pDeviceContext); // Uncomment for instanced rendering.
#pragma endregion
				}
				pPerTexSetIter++;
			}
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

	auto vShader = pShader->GetVertexShader();
	auto pixShader = pShader->GetPixelShader();
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
	resourceData.SysMemPitch = (UINT) (numLights * DirectionalLight::GetSize());
	resourceData.SysMemSlicePitch = (UINT) DirectionalLight::GetSize();
	resourceData.pSysMem = &this->m_Light;

	if (this->m_pFrameUniformStructuredBuffer)
		delete this->m_pFrameUniformStructuredBuffer;
	this->m_pFrameUniformStructuredBuffer = DirectX11Buffer::CreateStructuredBuffer(this->m_pDevice, numLights, DirectionalLight::GetSize(), true, false, &resourceData);

	ID3D11Buffer *pBuffers[] = { this->m_pObjectUniformBuffer->GetRawPointer(), this->m_pFrameUniformStructuredBuffer->GetRawPointer() };

	auto ptr = this->m_pFrameUniformStructuredBuffer->GetRawPointer();
	this->m_pDeviceContext->UpdateSubresource(ptr, 0, 0, pAsArray, 0, 0);
	this->m_pDeviceContext->PSSetConstantBuffers (2, 1, &ptr);

	this->UpdateFrameUniforms();
}

void
DirectX11Renderer::GetRenderablesByMesh(MeshAssetDescriptor * pMeshDesc, vector<Renderable *> * pRenderables)
{
	assert(pMeshDesc);
	assert(pRenderables);

	if (pMeshDesc && pRenderables)
	{
		for (auto renderableIterator = this->m_Renderables.begin(); renderableIterator != this->m_Renderables.end(); renderableIterator++)
		{
			DirectX11Renderable * pRenderable = (*renderableIterator);
			const Mesh * const  pMesh = pRenderable->GetMesh();
			if (pMesh->m_Name == pMeshDesc->GetName())
				pRenderables->push_back(pRenderable);
		}

	}
}

DirectX11Renderer::DirectX11Renderer(IEventManager * pManager) : IRenderer()
{
	assert(pManager);
	this->m_pEventManager = pManager;
	this->m_pFrameUniformStructuredBuffer = NULL;

	this->m_RenderTarget = NULL;
}


void
DirectX11Renderer::OnKeyDown(IEventData * pEvent)
{
	assert(pEvent);
	KeyDownEventData *pEventData = (KeyDownEventData *)pEvent;
	switch (pEventData->m_KeyCode)
	{
	case VK_UP:
		this->m_MoveBackForward += 10;
		break;
	case VK_DOWN:
		this->m_MoveBackForward -= 10;
		break;
	case VK_LEFT:
		this->m_MoveLeftRight	-= 10;
		break;
	case VK_RIGHT:
		this->m_MoveLeftRight	+= 10;
		break;
	}

}

void
DirectX11Renderer::OnAssetLoaded (IEventData * pEvent)
{
    assert (pEvent);
    if (pEvent)
    {
        CORE_BOOLEAN assetLoaded            = false;
        AssetLoadedEventData * pEventData   = (AssetLoadedEventData *)pEvent;
        AssetDescriptor * pDescriptor       = pEventData->m_pAssetDescriptorExtended;
        switch (pEventData->GetAssetType ())
        {
            case ASSET_TYPE_SHADER:
            {
                assetLoaded                 = this->LoadShader (pDescriptor);
                break;
            }
            case ASSET_TYPE_TEXTURE:
                assetLoaded                 = this->LoadTexture (pDescriptor);
                break;
            case ASSET_TYPE_MESH:
                assetLoaded                 = this->LoadMesh (pDescriptor);
                break;
			case ASSET_TYPE_MODEL:
				//assetLoaded					= this->LoadModel(pDescriptor);
				break;
			case ASSET_TYPE_PASS:
				assetLoaded					= this->LoadPass(pDescriptor);
				break;
			case ASSET_TYPE_MATERIAL:
				assetLoaded					= this->LoadMaterial (pDescriptor);
				break;
            default:
                assert (false);
        }

        if (assetLoaded == false)
        {
            AssetLoadFailedEventData * pAssetLoadFailedEvent    = new AssetLoadFailedEventData ();
            pAssetLoadFailedEvent->m_pAssetDescriptorExtended   = pDescriptor;
            this->m_pEventManager->VTriggerEvent (pAssetLoadFailedEvent);

        }
    }
}


void
DirectX11Renderer::OnEntityRegistered(IEventData * pEvent)
{
	assert(pEvent);
	if (pEvent)
	{
		CORE_BOOLEAN entityRegistered			= false;
		EntityRegisteredEventData * pEventData	= (EntityRegisteredEventData *)pEvent;
		CORE_ID entityID						= pEventData->m_Identifier;
		Entity * pEntity;
		CORE_ERROR err							= g_Engine.GetEntitySystem()->GetEntityByIdentifier(entityID, &pEntity);
		assert(err == ERR_OK);
		if (err == ERR_OK)
		{
		}
	}
}


void
DirectX11Renderer::OnEntityComponentRegistered (IEventData * pEvent)
{
	assert (pEvent);
	if (pEvent)
	{
		EntityComponentAddedEventData * pEventData	= (EntityComponentAddedEventData *)pEvent;
		if (pEventData->m_ComponentType == COMPONENT_TYPE_RENDERABLE)
		{
			RenderableComponent * pComponent		= (RenderableComponent *) pEventData->m_pComponent;
			assert (pComponent);

			auto pMaterialDesc	= pComponent->GetMaterial ();
			assert (pMaterialDesc);

			auto pAssetManager	= g_Engine.GetAssetManager ();
			
			Material * pMaterial = NULL;
			auto matId = pMaterialDesc->GetIdentifier ();
			auto pMatIt			= this->m_MaterialMap.find (pMaterialDesc->GetIdentifier ());
			if (pMatIt == this->m_MaterialMap.end ())
			{
				// Material not loaded. Load it. 
				pMaterial			= new Material ();
				Material::FromDescriptor (pMaterialDesc, pMaterial);
				auto pair			= make_pair (matId, pMaterial);
				this->m_MaterialMap.insert (pair);
			}
			else
				pMaterial = pMatIt->second;

			Entity * pEntity	= NULL;
			g_Engine.GetEntitySystem()->GetEntityByIdentifier(pEventData->m_EntityIdentifier, &pEntity);
			assert(pEntity);
			
			auto pModel								= pComponent->GetModel();
			for (size_t idx							= 0; idx < pModel->NumMeshes(); idx++)
			{
				auto pMesh							= pModel->GetMeshAt(idx);
				auto pMeshData						= pAssetManager->GetMesh(pMesh);
				vector<Renderable *> renderables;
				this->GetRenderablesByMesh(pMesh, &renderables);
				if (!renderables.size())
				{
					// Not loaded yet. Load it.

					auto pData			= pAssetManager->GetModelData(pModel->GetName());
					assert(pData);
								
					size_t numPasses	= pData->NumPasses();
					assert(pData->NumTextures());
					assert(numPasses);
					
					auto * pTex			= pData->GetTextureAt(0);
					DirectX11Texture2D * pDxTex = NULL;

					if (pTex)
					{
						auto findResult = this->m_TextureMap.find(pTex->GetIdentifier());
						pDxTex			= findResult->second;
					}

					DirectX11Shader * pPxShader = NULL;
					DirectX11Shader * pVxShader = NULL;
					for (size_t idx		= 1; idx < pData->NumShaders(); idx++)
					{
					/*
						* NOTE(Dino):
						* For now, our renderer doesn't understand multipass rendering.
						* To it, a Shader has a minimum of pixel and vertex shader.
						* But, out asset system does! And so, our model has TWO shaders instead of one.
						* This means that, left to its own devices, our renderer will happily create TWO renderables.
						* We'll skip this for now, until we teach our renderer what a pass is.
						*/
						auto pShader	= pData->GetShaderAt(idx);
						if (pShader)
						{
							auto pDxShader		= this->m_ShaderMap.find(pShader->GetIdentifier())->second;
							CORE_SHADER_TYPE type = pShader->GetShaderType ();
							if (type == CORE_SHADER_TYPE::SHADER_TYPE_PIXEL)
								pPxShader		= pDxShader;
							else if (type == CORE_SHADER_TYPE::SHADER_TYPE_VERTEX)
								pVxShader		= pDxShader;
							
							pComponent->GetMaterial ();
							auto pRenderable	= new DirectX11Renderable(pMeshData, pDxTex, pDxShader, false);
							this->m_Renderables.push_back(pRenderable);
							auto pair = make_pair(pModel->GetIdentifier(), pRenderable);
							this->m_ModelToRenderableMap.insert(pair);
							this->GetRenderablesByMesh(pMesh, &renderables);

						}
					}
				}

				{
					// The renderable is already registered. Now we can instantiate it.
					for (size_t idx					= 0; idx < renderables.size(); idx++)
					{
						auto pRenderable			= (DirectX11Renderable *) renderables.at(idx);
						//TODO(Dino): Get transform component instead of hardcoding matrix.
						this->m_RenderSet.Insert(pRenderable);
						
						auto * pComponent			= (TransformComponent *) pEntity->GetComponentByType (COMPONENT_TYPE_TRANSFORM);
						FASTMAT4 transformA			= FASTMAT_IDENTITY ();
						MAT4 transform;
						XMStoreFloat4x4(&transform, transformA);
						CreateMatFromTransform (pComponent, transform);
						transformA					= XMLoadFloat4x4 (&transform);
						// We rebuffer on every instance count change, to make sure the instance data is reloaded.
						auto pRenderableInstance	= pRenderable->Instantiate(pComponent->m_Identifier, transformA, pMaterial);
						if (pRenderable->GetInstanceCount () != 1)
							pRenderable->DeactivateBuffers ();
						pRenderable->Buffer(this->m_pDevice, this->m_pDeviceContext);
					}
				}
			}
		}
	}
}

void
CreateMatFromTransform (TransformComponent * pComponent, MAT4 &matP)
{
	FASTMAT4 mat = XMLoadFloat4x4(&matP);
	FASTMAT4 transform	= FASTMAT_IDENTITY ();
	auto rX				= XMMatrixRotationX (pComponent->m_Rotation.x);
	auto rY				= XMMatrixRotationY (pComponent->m_Rotation.y);
	auto rZ				= XMMatrixRotationZ (pComponent->m_Rotation.z);
	auto rotation		= XMMatrixMultiply ((rX, rY), rZ);
	auto translation	= XMMatrixTranslation (pComponent->m_Translation.x, pComponent->m_Translation.y, pComponent->m_Translation.z);
	auto scale			= XMMatrixScaling (pComponent->m_Scale.x, pComponent->m_Scale.y, pComponent->m_Scale.z);
	mat					= XMMatrixMultiply (XMMatrixMultiply(scale, rotation), translation );
	XMStoreFloat4x4(&matP, mat);
}
void
DirectX11Renderer::OnEntityComponentDeRegistered (IEventData * pEvent)
{
	assert (pEvent);
	if (pEvent)
	{
		EntityComponentRemovedEventData * pEventData = (EntityComponentRemovedEventData *)pEvent;
		if (pEventData->m_ComponentType == COMPONENT_TYPE_RENDERABLE)
		{
			RenderableComponent * pComponent = (RenderableComponent *)pEventData->m_pComponent;
			assert (pComponent);
			auto pModelDesc			= pComponent->GetModel();
			auto modelID			= pModelDesc->GetIdentifier();
			auto result				= this->m_ModelToRenderableMap.find(modelID);
			if ((*result).second)
			{
				auto pRenderable	= (*result).second;
				auto pInstances		= pRenderable->GetInstances ();
				pRenderable->DeInstantiate (pComponent->m_Identifier);
				pRenderable->Buffer(this->m_pDevice, this->m_pDeviceContext); // Rebuffer to reload instance data.
				if (!pRenderable->GetInstanceCount ())
				{
					this->m_ModelToRenderableMap.erase (modelID);
				
				
					for (auto it = this->m_Renderables.begin (); it != this->m_Renderables.end (); it++)
					{
						if ((*it) == pRenderable)
						{
							this->m_Renderables.erase (it);
							break;
						}
					}

					this->m_RenderSet.Remove(pRenderable);
					delete pRenderable;
				}
			}
		}
	}
}

void
DirectX11Renderer::OnEntityComponentChanged (IEventData * pEvent)
{
	assert (pEvent);
	if (pEvent)
	{
		auto pEventData			= (EntityComponentChangedEventData *)pEvent;
		if (pEventData->m_ComponentType == COMPONENT_TYPE_TRANSFORM)
		{
			auto pTransform		= (TransformComponent *) pEventData->m_pComponent;
			auto entityId		= pEventData->m_EntityIdentifier;
			
			Entity * pEntity	= NULL;
			g_Engine.GetEntitySystem ()->GetEntityByIdentifier (entityId, &pEntity);
			auto pCRenderable	= (RenderableComponent *) pEntity->GetComponentByType (COMPONENT_TYPE_RENDERABLE);
			if (pCRenderable)
			{
				auto pModel		 = pCRenderable->GetModel ();
				auto pRenderable = this->m_ModelToRenderableMap[pModel->GetIdentifier ()];
				auto pInstance    = pRenderable->GetInstance (pCRenderable->m_Identifier);
				if (pInstance)
				{
					auto transform = pInstance->GetTransform ();
					CreateMatFromTransform (pTransform, *transform);
					pInstance->RecomputeTransform ();
					/*NOTE(Dino): We have to rebuffer to update the instance data. */
					pRenderable->Buffer(this->m_pDevice, this->m_pDeviceContext);
				}
			}
		}
	}
}

CORE_BOOLEAN
DirectX11Renderer::LoadMesh (AssetDescriptor * pDescriptor)
{
	MeshAssetDescriptorExtended * pDescEx = NULL;
	assert(pDescriptor);
	if (pDescriptor)
	{
		pDescEx				= (MeshAssetDescriptorExtended *)pDescriptor;
		auto pMesh			= pDescEx->GetMesh();
		size_t numSubMeshes = pMesh->GetSubMeshCount();
		for (size_t idxSubs = 0; idxSubs < numSubMeshes; idxSubs++)
		{
			auto pSub = pMesh->GetSubMeshAt(idxSubs);
			
		}
	}

    return false;
}

CORE_BOOLEAN
DirectX11Renderer::LoadSubMesh(CoreMesh * pMesh, DirectX11Texture2D * pTexture, DirectX11Shader * pShader, Material * pMaterial)
{
	CORE_BOOLEAN isLoaded	= true;
	assert(pMesh);


	auto * pRenderable	= new DirectX11Renderable(pMesh, pTexture, pShader, pMaterial);
	this->m_Renderables.push_back(pRenderable);
	auto numSubs		= pMesh->GetSubMeshCount();
	for (auto idx		= 0; idx < numSubs; idx++)
	{
		auto pSubMesh	= pMesh->GetSubMeshAt(idx);
		isLoaded		&= this->LoadSubMesh(pMesh, pTexture, pShader, pMaterial);
		assert(isLoaded);
	}

	return isLoaded;
}

CORE_BOOLEAN
DirectX11Renderer::LoadTexture (AssetDescriptor * pDescriptor)
{
    CORE_BOOLEAN retVal                 = false;

    assert (pDescriptor);
    if (pDescriptor)
    {
		auto path = pDescriptor->GetPath();
        wstring wst(path.begin (), path.end ());
        DirectX11Texture2D  * pGrassTex = DirectX11Texture2D::FromDDSFile (this->m_pDevice, wst.c_str(), pDescriptor->GetIdentifier());
        assert (pGrassTex);
        if (pGrassTex)
        {
            retVal                      = true;
            this->RegisterTexture (pGrassTex);

        }
    }

    return retVal;
}


CORE_BOOLEAN
DirectX11Renderer::LoadPass(AssetDescriptor * pDescriptor)
{
	CORE_BOOLEAN retVal					= false;

	auto pPassDesc						= (ShaderPassDescriptor *) pDescriptor;
	CORE_ID pixId						= pPassDesc->GetPixelShader ();
	CORE_ID vertId						= pPassDesc->GetVertexShader ();
	
	return retVal;
}

CORE_BOOLEAN
DirectX11Renderer::LoadMaterial (AssetDescriptor * pDescriptor)
{
	CORE_BOOLEAN retVal					= false;
	
	auto pMaterialDesc					= (MaterialAssetDescriptor *)pDescriptor;
	auto id								= pMaterialDesc->GetIdentifier ();
	auto name							= pMaterialDesc->GetName ();

	Material * pMaterial				= new Material ();
	Material::FromDescriptor (pMaterialDesc, pMaterial);

	auto pair							= make_pair (id, pMaterial);
	this->m_MaterialMap.insert (pair);
	retVal								= ERR_OK;
	return retVal;
}

CORE_BOOLEAN 
DirectX11Renderer::LoadShader (AssetDescriptor * pDescriptor)
{
    CORE_BOOLEAN retVal                 = false;
    
	auto * pShaderDesc					= (ShaderAssetDescriptor *)pDescriptor;


    ShaderDescriptor shaderDesc;
    shaderDesc.m_Identifier             = pDescriptor->GetIdentifier ();
    string filePath                     = pDescriptor->GetPath ();
    wstring widePath (filePath.begin (), filePath.end ());
    shaderDesc.m_PixelShaderPath        = (wchar_t *)widePath.c_str ();
    shaderDesc.m_PixelShaderEntryPoint  = "PShader";
    shaderDesc.m_VertexShaderPath       = (wchar_t *)widePath.c_str ();
    shaderDesc.m_VertexShaderEntryPoint = "VShader";
 
    DirectX11Shader * pShader           = this->CreateShader (&shaderDesc);
    assert (pShader);
    if (pShader)
    {
        this->RegisterShader (pShader);
        retVal                          = true;
    }
    
    return retVal;
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
	this->m_Width       = width;
	this->m_Height      = height;
	/*NOTE(Dino): Changing the actual resolution is very, very hard. */
}