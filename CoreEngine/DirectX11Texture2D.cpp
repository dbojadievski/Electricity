#include "DirectX11Texture2D.h"
#include "DDSTextureLoader.h"

using namespace DirectX;
DirectX11Texture2D::DirectX11Texture2D()
{
	this->m_pResource = NULL;
	this->m_pSampler = NULL;
	this->m_pShaderResourceView = NULL;
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
	    pRetVal->m_Identifier = textureId;

end:
	return pRetVal;
}