#pragma once

#include "CoreTypes.h"
#include "IEngineSystem.h"
#include "IEventManager.h"
#include "EventManager.h"
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <map>

#include "DirectX11Renderable.h"
#include "DirectX11Shader.h"
#include "DirectX11Buffer.h"
#include "DirectX11Texture2D.h"
#include "DirectX11RenderSet.h"
#include "DirectX11TextRenderer.h"
#include "Mesh.h"
#include "Math.h"

#include "BasicLight.h"
#include "DirectionalLight.h"
#include "LightManager.h"

#include "AssetDescriptor.h"
#include "MeshAssetDescriptor.h"

#include "IRenderer.h"

struct ShaderDescriptor
{
	CORE_ID m_Identifier;

	LPWSTR m_VertexShaderPath;
	LPCSTR m_VertexShaderEntryPoint;

	LPWSTR m_PixelShaderPath;
	LPCSTR m_PixelShaderEntryPoint;

	ShaderDescriptor()
	{
	}

	ShaderDescriptor(CORE_ID identifier, LPCSTR pVertexShaderEntry, LPWSTR pVertexShaderPath, LPCSTR pPixelShaderEntry, LPWSTR pPixelShaderPath)
	{
		this->m_Identifier = identifier;
		
		this->m_VertexShaderEntryPoint = pVertexShaderEntry;
		this->m_VertexShaderPath = pVertexShaderPath;

		this->m_PixelShaderEntryPoint = pPixelShaderEntry;
		this->m_PixelShaderPath = pPixelShaderPath;
	}
};

class DirectX11Renderer : IRenderer
{
private:

	IEventManager * m_pEventManager;

	CORE_DOUBLE m_PrevFrameAt = 0;

	D3D_FEATURE_LEVEL m_featureLevel;

	IDXGISwapChain * m_pSwapChain;
	ID3D11Device * m_pDevice;
	ID3D11DeviceContext * m_pDeviceContext;
	ID3D11RenderTargetView * m_pRenderTargetViewBackBuffer;
	ID3D11RenderTargetView * m_pRenderTargetViewTex;
	DirectX11Texture2D * m_RenderTarget; // Used to render to texture.
	ID3D11ShaderResourceView* m_pShaderResourceView;

	ID3D11InputLayout * m_pInputLayout;
	ID3D11Texture2D* m_pDepthStencilBuffer;
	ID3D11DepthStencilView* m_pDepthStencilViewBackBuffer;
	ID3D11DepthStencilView* m_pDepthStencilViewTex;
	ID3D11RasterizerState * m_pRasterizerStateWireframe;

	ID3D11BlendState* m_pBlendStateTransparency;
	ID3D11RasterizerState* m_pCounterClockWisecullMode;
	ID3D11RasterizerState* m_pClockWiseCullMode;
	ID3D11RasterizerState* m_pDisableCullingMode;

	DirectX11TextRenderer * m_pTextRenderer;

	DirectX11Shader * m_pActiveShader;
	map<CORE_ID, DirectX11Shader *> m_ShaderMap;
	DirectX11Shader * m_pRedShader;
	DirectX11Shader * m_pGreenShader;
    DirectX11Shader * m_pBasicShader;

	DirectX11Texture2D * m_pActiveTexture;
	DirectX11Texture2DMap m_TextureMap;
	vector<DirectX11Renderable *> m_Renderables;
	map<CORE_ID, DirectX11Renderable *> m_ModelToRenderableMap;
	DirectX11RenderSet m_RenderSet;

	VEC4 m_ClearColour;
	vector<DirectionalLight> m_Lights;
	LightManager m_LightManager;

	MAT4 m_WorldViewProjection;
	
	MAT4 m_CameraView;
	MAT4 m_CameraProjection;
	VEC4 m_CameraPosition;
	MAT4 m_CameraRotation;
	VEC4 m_CameraTarget;
	VEC4 m_CameraUp;
	VEC4 m_CameraRight;
	VEC4 m_CameraForward;
	CORE_REAL m_CameraPitch;
	CORE_REAL m_CameraYaw;
	CORE_REAL m_MoveLeftRight;
	CORE_REAL m_MoveBackForward;

	CORE_DWORD m_Width;
	CORE_DWORD m_Height;

	D3D11_INPUT_ELEMENT_DESC m_inputElementDescriptor[8]
		=
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "RowX",		0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA,	 1},
		{ "RowY",		0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "RowZ",		0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "RowW",		0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1}
	};


	struct InstanceUniformDescriptor
	{
		MAT4 WorldViewProjection;
		MAT4 World;
	};

	struct FrameUniformDescriptor
	{
		DirectionalLight light;
		MAT4 Camera;
		MAT4 ViewProjectionMatrix;
	};


	DirectionalLight m_Light;
	FrameUniformDescriptor m_FrameUniforms;
	InstanceUniformDescriptor m_PerObjectBuffer;
	DirectX11Buffer * m_pUniformBuffer;
	DirectX11Buffer * m_pFrameUniformBuffer;
	DirectX11Buffer * m_pFrameUniformStructuredBuffer;
	DirectX11Buffer * m_pRgbBuffer;

	void InitEventHandlers();
	void InitDirect3D();
	void InitTextRenderer();
	void InitBlendStates();
	void SetRasterizerToWireFrame();
	void InitShaders();
	DirectX11Shader * CreateShader(ShaderDescriptor * pDescriptor);
	void InitTerrain();
	void InitTextures();
	void InitLights();
    void InitFrameUniformBuffer ();
	void ReloadLightBuffer();
	CORE_BOOLEAN InitRenderTarget ();
	CORE_BOOLEAN InitCamera();
	CORE_BOOLEAN InitDepthBuffer();

	void UpdateCamera();
	void UpdateFrameUniforms();

	void SetTexture(DirectX11Texture2D * pTex);
	void SetShader(DirectX11Shader * pShader);
	void ResetBlendState();
	void SetBlendStateTransparent();

	void RegisterShader(DirectX11Shader * pShader);
	void DeRegisterShader(CORE_ID shaderId);
		
	void RegisterTexture(DirectX11Texture2D * pTexture);
	void DeRegisterTexture(CORE_ID textureId);

	void CloseDirectX11Device();

	void ClearRenderTargetTex ();
	void ClearRenderTargetBackBuffer ();

	void RenderAll(CORE_DOUBLE dT);
	void RenderAllSimple(CORE_DOUBLE dT);
	void RenderAllInSet(DirectX11RenderableMap * pMap, size_t &numShaderSwitches, size_t &numTextureSwitches, size_t &numRenderableInstances, const MAT4 &cameraViewProjectionMatrix);
	
	/*Simple fetch functions.*/
	virtual void GetRenderablesByMesh(MeshAssetDescriptor * pMeshDesc, vector<Renderable *> * pRenderables);

	/*Event handlers. */
	void OnResolutionChanged(IEventData * pData);
	void OnKeyDown(IEventData * pData);
    
	void OnAssetLoaded (IEventData * pEvent);
	
	void OnEntityRegistered(IEventData * pEvent);
	void OnEntityDeRegistered(IEventData * pEvent);

	void OnEntityComponentRegistered (IEventData * pEvent);
	void OnEntityComponentDeRegistered (IEventData * pEvent);
	void OnEntityComponentChanged (IEventData * pEvent);

    CORE_BOOLEAN LoadShader (AssetDescriptor * pDescriptor);
    CORE_BOOLEAN LoadTexture (AssetDescriptor * pDescriptor);
	CORE_BOOLEAN LoadSubMesh(CoreMesh * pMesh, DirectX11Texture2D * pTexture, DirectX11Shader * pShader);
    CORE_BOOLEAN LoadMesh (AssetDescriptor * pDescriptor);
	CORE_BOOLEAN LoadPass (AssetDescriptor * pDescriptor);
	CORE_BOOLEAN LoadModel (AssetDescriptor * pDescriptor);

public:
	DirectX11Renderer(IEventManager * pEventManager);
	virtual void Init();
	virtual void Update(CORE_DOUBLE dT);
	virtual void ShutDown();
	
	void ChangeResolution(CORE_DWORD width, CORE_DWORD height);
};