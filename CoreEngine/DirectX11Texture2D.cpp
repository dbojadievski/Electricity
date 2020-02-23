#include "DirectX11Texture2D.h"
#include "DDSTextureLoader.h"

using namespace DirectX;
DirectX11Texture2D::DirectX11Texture2D()
{
	this->m_pResource = NULL;
	this->m_pSampler = NULL;
	this->m_pShaderResourceView = NULL;
	this->m_pTex = NULL;
}


DirectX11Texture2D *
DirectX11Texture2D::AsRenderTarget (ID3D11Device * pDevice, CORE_ULONG width, CORE_ULONG height, CORE_BYTE mipLevels, CORE_BYTE sampleCount)
{
	DirectX11Texture2D * pRetVal	= new DirectX11Texture2D ();
	pRetVal->m_Identifier			= 0;
	assert (pDevice);

	HRESULT result;
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory (&textureDesc, sizeof (textureDesc));
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = mipLevels;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = sampleCount;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	result = pDevice->CreateTexture2D (&textureDesc, NULL, &(pRetVal->m_pTex));
	assert (result == S_OK);
	if (result != S_OK)
	{
		delete pRetVal;
		pRetVal = NULL;
	}
end:
	return pRetVal;
}

DirectX11Texture2D * 
DirectX11Texture2D::FromDDSFile(ID3D11Device * pDevice, const wchar_t * szFileName, CORE_ID textureId)
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
	else
	{
		pRetVal->m_Identifier = textureId;
		D3D11_SAMPLER_DESC desc;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.BorderColor[0] = 0;
		desc.BorderColor[1] = 0;
		desc.BorderColor[2] = 0;
		desc.BorderColor[3] = 0;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		pDevice->CreateSamplerState (&desc, &pRetVal->m_pSampler);
	}

end:
	return pRetVal;
}

ID3D11Texture2D * 
DirectX11Texture2D::GetRawPointer ()
{
	return this->m_pTex;
}

DirectX11Texture2D::~DirectX11Texture2D ()
{
	this->m_pResource->Release ();
	this->m_pResource = NULL;

	this->m_pSampler->Release ();
	this->m_pSampler = NULL;

	this->m_pShaderResourceView->Release ();
	this->m_pShaderResourceView = NULL;
	
	//this->m_pTex->Release ();
	//this->m_pTex = NULL;

}