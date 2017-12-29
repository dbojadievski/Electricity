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
	DirectX11Texture2D();
public:
	static DirectX11Texture2D * FromDDSFile(ID3D11Device * pDevice, const wchar_t * szFileName, CORE_ID textureId);
};

typedef map<CORE_ID, DirectX11Texture2D *> DirectX11Texture2DMap;