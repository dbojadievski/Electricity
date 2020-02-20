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
#include "CoreEngine.h"

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")

#define VIEWPORT_DEPTH_MIN 0.0f
#define VIEWPORT_DEPTH_MAX 1.0f

#define VIEW_NEAR 1.0f
#define VIEW_FAR 1000.0f

#define RES_WIDTH 1920.0f
#define RES_HEIGHT 1080.0f
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
	InitTerrain();
    InitLights ();
}

void 
DirectX11Renderer::InitLights()
{
	this->m_Light.m_Direction           = FLOAT3(0.25f, 0.5f, -1.0f);
	this->m_Light.m_ColourAmbient       = FLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	this->m_Light.m_ColourDiffuse       = FLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	this->m_Lights.push_back(this->m_Light);
	this->m_LightManager.AddDirectionalLight(&this->m_Light);

	this->ReloadLightBuffer();
}

void DirectX11Renderer::InitFrameUniformBuffer ()
{
    this->m_pFrameUniformBuffer = DirectX11Buffer::CreateConstantBuffer (this->m_pDevice, sizeof (FrameUniformDescriptor), true, false, NULL);
    assert (this->m_pFrameUniformBuffer);
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

end:
	return isSuccess;
}

void 
DirectX11Renderer::InitDirect3D()
{
#pragma region Initialize swap chain.
	DXGI_SWAP_CHAIN_DESC swapChainDescriptor;
	ZeroMemory(&swapChainDescriptor, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDescriptor.BufferCount = 1;
	swapChainDescriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // We're using standard 32-bit colours for now. Would need to be increased for HDR support.
	swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescriptor.OutputWindow = g_Window;
	swapChainDescriptor.SampleDesc.Count = 4; //TODO(Dino): Make configurable.
	//swapChainDescriptor.Quality = 16;
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
		D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_SINGLETHREADED,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&swapChainDescriptor,
		&this->m_pSwapChain,
		&this->m_pDevice,
		&this->m_featureLevel,
		&this->m_pDeviceContext);
#pragma endregion
#pragma region Initialize back-buffer and render target.
	///*Sets the backbuffer to be the current render target. */
	ID3D11Texture2D *pBackBuffer;
	this->m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *) &pBackBuffer);
	this->m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &this->m_pRenderTargetViewBackBuffer);
	pBackBuffer->Release();
	this->m_pDeviceContext->OMSetRenderTargets(1, &this->m_pRenderTargetViewBackBuffer, NULL);
	
	// Instead of setting the back-buffer, we now render to texture.

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
	wfdesc.DepthClipEnable = 0;
	this->m_pDevice->CreateRasterizerState(&wfdesc, &this->m_pRasterizerStateWireframe);
	this->m_pDeviceContext->RSSetState(NULL);
#pragma endregion

	this->m_ClearColour = { 0.1f, 0.1f, 0.1f, 1.0f };

	InitDepthBuffer();
	this->InitRenderTarget ();
	InitShaders();
	InitTextures();
	InitCamera();
	InitBlendStates();
	InitTextRenderer();
    InitFrameUniformBuffer ();
}

void
DirectX11Renderer::ClearRenderTargetTex ()
{
	float backgroundColor[4] = { this->m_ClearColour.x, this->m_ClearColour.y, this->m_ClearColour.z, this->m_ClearColour.w };
	this->m_pDeviceContext->ClearRenderTargetView (this->m_pRenderTargetViewTex, backgroundColor);
	this->m_pDeviceContext->ClearDepthStencilView (this->m_pDepthStencilViewTex, D3D11_CLEAR_DEPTH, 1.0f, 0);
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
	depthStencilDesc.SampleDesc.Count = 4;
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

	result = this->m_pDevice->CreateDepthStencilView(this->m_pDepthStencilBuffer, NULL, &this->m_pDepthStencilViewBackBuffer);
	if (result != S_OK)
	{
		wasInitted = false;
		goto end;
	}

	this->m_pDeviceContext->OMSetRenderTargets(1, &this->m_pRenderTargetViewBackBuffer, this->m_pDepthStencilViewBackBuffer);


end:
	return wasInitted;
}

void 
DirectX11Renderer::InitShaders()
{
	this->m_pActiveShader = NULL;

	DirectX11Shader * pShader = new DirectX11Shader(L"Assets\\Shaders\\red.hlsl", "VShader", L"Assets\\Shaders\\red.hlsl", "PShader", 1);
	
	ID3D11VertexShader ** pVertexShader = pShader->GetVertexShader();
	DirectXShaderBufferDescriptor vertexShaderDescriptor = pShader->GetVertexShaderBufferPointer();
	HRESULT errVsh	= this->m_pDevice->CreateVertexShader(vertexShaderDescriptor.m_pBuffer, vertexShaderDescriptor.m_size, NULL, pVertexShader);
	this->m_pDeviceContext->VSSetShader(*pVertexShader, 0, 0);

	ID3D11PixelShader ** pPixelShader = pShader->GetPixelShader();
	DirectXShaderBufferDescriptor pixelShaderDescriptor = pShader->GetPixelShaderBufferPointer();
	HRESULT errPsh	= this->m_pDevice->CreatePixelShader(pixelShaderDescriptor.m_pBuffer, pixelShaderDescriptor.m_size, NULL, pPixelShader);
	this->m_pDeviceContext->PSSetShader(*pPixelShader, 0, 0);


	DirectXShaderBufferDescriptor descriptor = pShader->GetVertexShaderBufferPointer();
	ID3D11InputLayout * pLayout = NULL;
	HRESULT res = this->m_pDevice->CreateInputLayout(this->m_inputElementDescriptor, ARRAYSIZE(m_inputElementDescriptor), descriptor.m_pBuffer, descriptor.m_size, &pLayout);
	pShader->SetInputLayout(pLayout);
	this->m_pDeviceContext->IASetInputLayout(pLayout);

	this->RegisterShader(pShader);
	this->m_pActiveShader = pShader;

    ShaderDescriptor * pTexturingShader = new ShaderDescriptor (13, "VShader", L"Assets\\Shaders\\shaders.hlsl", "PShader", L"Assets\\Shaders\\shaders.hlsl");
    DirectX11Shader * pTexShader        = this->CreateShader (pTexturingShader);
    this->m_pBasicShader                = pTexShader;
    this->RegisterShader (pTexShader);
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
DirectX11Renderer::InitTerrain()
{
	CORE_REAL terrainWidth = 100.0f;
	CORE_REAL terrainHeight = 100.0f;

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

	DirectX11RenderableInstance * pDynamicTerrainInstance = pDynamicTerrainRenderable->Instantiate(5, XMMatrixScaling(10.0f, 10.0f, 10.0f), NULL);
	assert(pDynamicTerrainInstance);
 	pDynamicTerrainRenderable->Buffer(this->m_pDevice, this->m_pDeviceContext);
	this->m_RenderSet.Insert(pDynamicTerrainRenderable);
}


void 
DirectX11Renderer::InitTextures()
{
	DirectX11Texture2D * pTex = DirectX11Texture2D::FromDDSFile(this->m_pDevice, L"Assets\\Textures\\wood.dds", 1);
	this->RegisterTexture(pTex);

	DirectX11Texture2D  * pGrassTex = DirectX11Texture2D::FromDDSFile(this->m_pDevice,  L"Assets\\Textures\\grass.dds", 2);
	this->RegisterTexture(pGrassTex);
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
	
	this->m_pUniformBuffer = DirectX11Buffer::CreateConstantBuffer(this->m_pDevice, sizeof(InstanceUniformDescriptor), true, false, NULL);
	assert(this->m_pUniformBuffer);

	this->m_CameraYaw		= 0;
	this->m_CameraPitch		= 0;
	this->m_MoveLeftRight	= 0.0f;
	this->m_MoveBackForward = 0.0f;

	this->m_CameraPosition	=  VEC4(0.0f, 3.0f, -8.0f, 0.0f);
	this->m_CameraTarget	= VEC4(0.0f, -1.0f, 0.0f, 0.0f);
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

	FASTVEC DOWN            = FASTVEC_SET(0.0f, -1.0f, 0.0f, 0.0f);
	/*DEBUG(Dino): The next line is code to debug terrain. Remove afterwards. */
	//this->m_CameraTarget = DOWN;
	//this->m_CameraView      = XMMatrixLookAtLH(this->m_CameraPosition, this->m_CameraTarget, this->m_CameraUp);
}

void 
DirectX11Renderer::UpdateFrameUniforms()
{
	auto pUniformBuffer     = this->m_pFrameUniformBuffer->GetRawPointer();
	this->m_pDeviceContext->UpdateSubresource(pUniformBuffer, 0, NULL, &this->m_FrameUniforms, 0, 0);
	this->m_pDeviceContext->PSSetConstantBuffers(0, 1, &pUniformBuffer);
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
	this->m_pDepthStencilViewTex->Release();
	this->m_pDepthStencilBuffer->Release();
	delete this->m_pUniformBuffer;
	delete this->m_pFrameUniformBuffer;
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
	//this->ClearRenderTargetTex (); //Enable when rendering to texture.
	this->ClearRenderTargetBackBuffer ();

	FASTMAT4 cameraView = XMLoadFloat4x4(&this->m_CameraView);
	FASTMAT4 cameraProjection = XMLoadFloat4x4(&this->m_CameraProjection);
	FASTMAT4 wp = cameraView * cameraProjection;
	MAT4 cameraViewProjectionMatrix;
	XMStoreFloat4x4(&cameraViewProjectionMatrix, wp);

	/*NOTE(Dino): Update per-frame uniform buffer. */
	this->m_FrameUniforms.light = this->m_Light;
	this->UpdateFrameUniforms();

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

	//ClearRenderTargetTex() before rendering to texture.
	this->ClearRenderTargetBackBuffer ();
	auto cameraView						= XMLoadFloat4x4(&this->m_CameraView);
	auto cameraProjection				= XMLoadFloat4x4(&this->m_CameraProjection);

	FASTMAT4 cameraViewProjectionMatrix = cameraView * cameraProjection;
	MAT4 viewProjectionMatrix;
	XMStoreFloat4x4(&viewProjectionMatrix, cameraViewProjectionMatrix);

	/*NOTE(Dino): Update per-frame uniform buffer. */
	this->m_FrameUniforms.light = this->m_Light;
	this->UpdateFrameUniforms();

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

		DirectX11RenderablesPerTextureSet * pPerTexSet = &(*perShaderIterator->second);
		auto pPerTexSetIter = pPerTexSet->begin();
		if (pPerTexSetIter != pPerTexSet->end ())
		{
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
					if (pRenderable->GetInstanceCount() == 0)
						continue;
					pRenderable->ActivateBuffers(this->m_pDeviceContext);
#pragma region Segment 1: Instanced rendering.
					DirectX11RenderableInstanceIterator instanceIterator = pRenderable->GetInstances ();
					while (instanceIterator != pRenderable->GetInstancesEnd ())
					{
						// Fill up the instance buffer.

						auto pRenderableInstance		= *instanceIterator;
						auto  pInstanceTransform		= pRenderableInstance->GetCachedTransform ();

						auto instanceTransform			= XMLoadFloat4x4(pInstanceTransform);
						auto cameraTransform			= XMLoadFloat4x4(&this->m_CameraView);
						auto cwp						= XMLoadFloat4x4(&cameraViewProjectionMatrix);


						auto modelViewProjectionMatrix	= instanceTransform * cameraTransform * cwp;
						
						auto wvp						= FASTMAT_TRANSPOSE(modelViewProjectionMatrix);
						auto itt						= FASTMAT_TRANSPOSE(instanceTransform);
						XMStoreFloat4x4(&this->m_PerObjectBuffer.WorldViewProjection, wvp);
						XMStoreFloat4x4(&this->m_PerObjectBuffer.World, itt);

						instanceIterator++;
					}
					//this->m_PerObjectBuffer.World				= FASTMAT_IDENTITY ();
					//this->m_PerObjectBuffer.WorldViewProjection = FASTMAT_IDENTITY ();

					auto cameraTransform						= XMLoadFloat4x4(&this->m_CameraView);
					auto cwp									= XMLoadFloat4x4(&cameraViewProjectionMatrix);
					auto vpm = (cameraTransform * cwp);
					XMStoreFloat4x4(&this->m_FrameUniforms.ViewProjectionMatrix, vpm);
					this->m_FrameUniforms.Camera				= this->m_CameraView;
					auto pUniformBufferPointer						= this->m_pUniformBuffer->GetRawPointer ();
					this->m_pDeviceContext->UpdateSubresource (pUniformBufferPointer, 0, NULL, &this->m_PerObjectBuffer, 0, 0);
					this->m_pDeviceContext->VSSetConstantBuffers (0, 1, &pUniformBufferPointer);
					pRenderable->Render (this->m_pDeviceContext);
#pragma endregion
#pragma region Segment 2: Basic direct rendering.
					//DirectX11RenderableInstanceIterator instanceIterator = pRenderable->GetInstances();
					//while (instanceIterator != pRenderable->GetInstancesEnd())
					//{
					//	++numRenderableInstances;
					//	auto pRenderableInstance		= *instanceIterator;
					//	auto  pInstanceTransform		= pRenderableInstance->GetCachedTransform();
					//	auto modelViewProjectionMatrix	= *pInstanceTransform * this->m_CameraView * cameraViewProjectionMatrix;
					//	this->m_PerObjectBuffer.WorldViewProjection = FASTMAT_TRANSPOSE(modelViewProjectionMatrix);
					//	this->m_PerObjectBuffer.World	=  FASTMAT_TRANSPOSE(*pInstanceTransform);
					//	this->m_PerObjectBuffer.Camera	= this->m_CameraView;
					//	auto pUniformBufferPointer		= this->m_pUniformBuffer->GetRawPointer();
					//	this->m_pDeviceContext->UpdateSubresource(pUniformBufferPointer, 0, NULL, &this->m_PerObjectBuffer, 0, 0);
					//	this->m_pDeviceContext->VSSetConstantBuffers(0, 1, &pUniformBufferPointer);

					//	assert(pRenderableInstance);
					//	instanceIterator++;
					//	pRenderable->Render(this->m_pDeviceContext);
					//}
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
	return;
	DirectionalLight * pAsArray = this->m_Lights.data();
	size_t numLights = this->m_Lights.size();

	D3D11_SUBRESOURCE_DATA resourceData;
	resourceData.SysMemPitch = (UINT) (numLights * sizeof(DirectionalLight));
	resourceData.SysMemSlicePitch = sizeof(DirectionalLight);
	resourceData.pSysMem = &this->m_Light;

	if (this->m_pFrameUniformStructuredBuffer)
		delete this->m_pFrameUniformStructuredBuffer;
	this->m_pFrameUniformStructuredBuffer = DirectX11Buffer::CreateStructuredBuffer(this->m_pDevice, numLights, sizeof(DirectionalLight), true, false, &resourceData);

	ID3D11Buffer *pBuffers[] = { this->m_pUniformBuffer->GetRawPointer(), this->m_pFrameUniformStructuredBuffer->GetRawPointer() };

	auto ptr = this->m_pFrameUniformStructuredBuffer->GetRawPointer();
	this->m_pDeviceContext->UpdateSubresource(ptr, 0, 0, pAsArray, 0, 0);
	this->m_pDeviceContext->PSSetConstantBuffers(0, ARRAYSIZE(pBuffers), pBuffers);

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
		this->m_MoveBackForward += 1;
		break;
	case VK_DOWN:
		this->m_MoveBackForward -= 1;
		break;
	case VK_LEFT:
		break;
	case VK_RIGHT:
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

			Entity * pEntity	= NULL;
			g_Engine.GetEntitySystem()->GetEntityByIdentifier(pEventData->m_EntityIdentifier, &pEntity);
			assert(pEntity);
			
			auto pModel								= pComponent->GetModel();
			for (size_t idx							= 0; idx < pModel->NumMeshes(); idx++)
			{
				auto pMesh							= pModel->GetMeshAt(idx);
				auto pMeshData						= g_Engine.GetAssetManager()->GetMesh(pMesh);
				vector<Renderable *> renderables;
				this->GetRenderablesByMesh(pMesh, &renderables);
				if (!renderables.size())
				{
					// Not loaded yet. Load it.

					auto pData			= g_Engine.GetAssetManager()->GetModelData(pModel->GetName());
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
						auto pShader	= pData->GetShaderAt(idx);
						if (pShader)
						{
							auto pDxShader		= this->m_ShaderMap.find(pShader->GetIdentifier())->second;
							CORE_SHADER_TYPE type = pShader->GetShaderType ();
							if (type == CORE_SHADER_TYPE::SHADER_TYPE_PIXEL)
								pPxShader		= pDxShader;
							else if (type == CORE_SHADER_TYPE::SHADER_TYPE_VERTEX)
								pVxShader		= pDxShader;

							auto pRenderable	= new DirectX11Renderable(pMeshData, pDxTex, pDxShader);
							this->m_Renderables.push_back(pRenderable);
							auto pair = make_pair(pModel->GetIdentifier(), pRenderable);
							this->m_ModelToRenderableMap.insert(pair);
							this->GetRenderablesByMesh(pMesh, &renderables);

							/*
							 * NOTE(Dino):
							 * For now, our renderer doesn't understand multipass rendering.
							 * To it, a Shader has a minimum of pixel and vertex shader.
							 * But, out asset system does! And so, our model has TWO shaders instead of one.
							 * This means that, left to its own devices, our renderer will happily create TWO renderables.
							 * We'll skip this for now, until we teach our renderer what a pass is.
							 */
						}
					}

					for (size_t passIdx = 0; passIdx < numPasses; passIdx++)
					{
						DirectX11Renderable * pRenderable = NULL;
						auto * pPass	= pData->GetPassAt(passIdx);
						auto pShader	= pPass->GetPixelShader();
						auto vShader	= pPass->GetVertexShader();
						
						for (auto it = this->m_ShaderMap.begin (); it != this->m_ShaderMap.end (); it++)
						{
							auto pSh = (*it).second;
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
						FASTMAT4 transformA	= FASTMAT_IDENTITY ();
						MAT4 transform;
						XMStoreFloat4x4(&transform, transformA);
						CreateMatFromTransform (pComponent, transform);
						// We rebuffer on every instance count change, to make sure the instance data is reloaded.
						auto pRenderableInstance	= pRenderable->Instantiate(pComponent->m_Identifier, transformA);
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
	mat					= XMMatrixMultiply (scale, XMMatrixMultiply (translation, rotation));
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
DirectX11Renderer::LoadSubMesh(CoreMesh * pMesh, DirectX11Texture2D * pTexture, DirectX11Shader * pShader)
{
	CORE_BOOLEAN isLoaded	= true;
	assert(pMesh);


	auto * pRenderable	= new DirectX11Renderable(pMesh, pTexture, pShader);
	this->m_Renderables.push_back(pRenderable);
	auto numSubs		= pMesh->GetSubMeshCount();
	for (auto idx		= 0; idx < numSubs; idx++)
	{
		auto pSubMesh	= pMesh->GetSubMeshAt(idx);
		isLoaded		&= this->LoadSubMesh(pMesh, pTexture, pShader);
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