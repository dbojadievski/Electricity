#pragma once

#include <d3d11.h>
#include "CoreTypes.h"
#include <assert.h>
#include <map>

using namespace std;
class DirectX11Texture2D
{

public:
	ID3D11Resource * m_pResource;
	ID3D11SamplerState * m_pSampler;
	ID3D11ShaderResourceView * m_pShaderResourceView;

	CORE_ID m_Identifier;
private:
	ID3D11Texture2D * m_pTex;
	DirectX11Texture2D ();
public:
	static DirectX11Texture2D * FromDDSFile(ID3D11Device * pDevice, const wchar_t * szFileName, CORE_ID textureId);
	static DirectX11Texture2D * AsRenderTarget (ID3D11Device * pDevice, CORE_ULONG width, CORE_ULONG height, CORE_BYTE mipLevels = 1, CORE_BYTE sampleCount = 0);
	ID3D11Texture2D * GetRawPointer ();
	~DirectX11Texture2D ();
};

typedef map<CORE_ID, DirectX11Texture2D *> DirectX11Texture2DMap;