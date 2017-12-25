#pragma once

#include <d3d11.h>
#include "CoreTypes.h"
#include <assert.h>
#include "DDSTextureLoader.h"
#include <map>
class DirectX11Texture2D
{

public:
	ID3D11Resource * m_pResource;
	ID3D11SamplerState * m_pSampler;
	ID3D11ShaderResourceView * m_pShaderResourceView;

	CORE_ID m_Identifier;
private:
	DirectX11Texture2D()
	{
		this->m_pResource = NULL;
		this->m_pSampler = NULL;
		this->m_pShaderResourceView = NULL;
	}
public:
	static DirectX11Texture2D * FromDDSFile(ID3D11Device * pDevice, const wchar_t * szFileName, CORE_ID textureId)
	{
		DirectX11Texture2D * pRetVal = new DirectX11Texture2D();
		
		assert(pDevice);
		assert(szFileName);

		HRESULT texResult = CreateDDSTextureFromFile(pDevice, szFileName, &pRetVal->m_pResource, &pRetVal->m_pShaderResourceView);
		assert(texResult == S_OK);
		if (texResult != S_OK)
		{
			pRetVal->m_pSampler->Release();
			pRetVal->m_pShaderResourceView->Release();
			
			delete pRetVal;
			pRetVal = NULL;
			
			goto end;
		}

		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		texResult = pDevice->CreateSamplerState(&sampDesc, &pRetVal->m_pSampler);
		
		pRetVal->m_Identifier = textureId;
		
	end:
		return pRetVal;
	}
};

typedef map<CORE_ID, DirectX11Texture2D *> DirectX11Texture2DMap;