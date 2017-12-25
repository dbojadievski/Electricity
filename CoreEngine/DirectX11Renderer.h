#pragma once

#include "CoreTypes.h"
#include "IEngineSystem.h"

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

class DirectX11Renderer : IEngineSystem
{
private:
	D3D_FEATURE_LEVEL m_featureLevel;

	IDXGISwapChain * m_pSwapChain;
	ID3D11Device * m_pDevice;
	ID3D11DeviceContext * m_pDeviceContext;
	ID3D11RenderTargetView * m_pRenderTargetView;
	
	ID3D11InputLayout * m_pInputLayout;
	ID3D11Texture2D* m_pDepthStencilBuffer;
	ID3D11DepthStencilView* m_pDepthStencilView;
	ID3D11RasterizerState * m_pRasterizerStateWireframe;

	ID3D11BlendState* m_pBlendStateTransparency;
	ID3D11RasterizerState* m_pCounterClockWisecullMode;
	ID3D11RasterizerState* m_pClockWiseCullMode;
	ID3D11RasterizerState* m_pDisableCullingMode;

	DirectX11TextRenderer * m_pTextRenderer;

	DirectX11Shader * m_pActiveShader;
	map<CORE_ID, DirectX11Shader *> m_ShaderMap;

	DirectX11Texture2D * m_pActiveTexture;
	DirectX11Texture2DMap m_TextureMap;
	vector<DirectX11Renderable *> m_pRenderables;
	DirectX11RenderSet m_RenderSet;

	VEC4 m_ClearColour;

	FASTMAT4 m_World;
	FASTMAT4 m_WorldViewProjection;
	
	FASTMAT4 m_CameraView;
	FASTMAT4 m_CameraProjection;
	FASTVEC m_CameraPosition;
	FASTVEC m_CameraTarget;
	FASTVEC m_CameraUp;

	struct InstanceUniformDescriptor
	{
		FASTMAT4 WorldViewProjection;
	};

	InstanceUniformDescriptor m_PerObjectBuffer;
	DirectX11Buffer * m_pUniformBuffer;

	void InitDirect3D();
	void InitTextRenderer();
	void InitBlendStates();
	void SetRasterizerToWireFrame();
	void InitShader();
	void InitVertexBuffer();
	CORE_BOOLEAN InitTransformationPipeline();
	CORE_BOOLEAN InitDepthBuffer();
	
	void SetTexture(DirectX11Texture2D * pTex);
	void SetShader(const DirectX11Shader * pShader);
	void ResetBlendState();
	void SetBlendStateTransparent();

	void RegisterShader(DirectX11Shader * pShader);
	void DeRegisterShader(CORE_ID shaderId);
	
	void RegisterTexture(DirectX11Texture2D * pTexture);
	void DeRegisterTexture(CORE_ID textureId);

	void CloseDirectX11Device();

	void RenderAll();
	void RenderAllInSet(DirectX11RenderableMap * pMap, size_t &numShaderSwitches, size_t &numTextureSwitches, size_t &numRenderableInstances, const FASTMAT4 &cameraViewProjectionMatrix);
public:
	void Init();
	void Update(float dT);
	void ShutDown();
};