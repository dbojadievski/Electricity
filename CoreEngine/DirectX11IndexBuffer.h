#pragma once

#include <assert.h>
#include "CoreTypes.h"
#include <d3d11.h>

class DirectX11IndexBuffer
{
private:
	ID3D11Buffer * m_pBuffer;
	D3D11_BUFFER_DESC m_Descriptor;
	D3D11_SUBRESOURCE_DATA m_InitData;

	DirectX11IndexBuffer() {}
public:
	DirectX11IndexBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, UINT byteWidth, unsigned int * pData);
	ID3D11Buffer * GetRawPointer();
	
	static DirectX11IndexBuffer *
	CreateIndexBuffer(ID3D11Device * pDevice, UINT size, CORE_BOOLEAN isDynamic, D3D11_SUBRESOURCE_DATA * pInitialData)
	{
		assert(pDevice);
		assert(size);

		DirectX11IndexBuffer * pRetVal = new DirectX11IndexBuffer();

		pRetVal->m_Descriptor.ByteWidth = size;
		pRetVal->m_Descriptor.MiscFlags = 0;
		pRetVal->m_Descriptor.StructureByteStride = 0;
		pRetVal->m_Descriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;

		if (isDynamic)
		{
			pRetVal->m_Descriptor.Usage = D3D11_USAGE_DYNAMIC;
			pRetVal->m_Descriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		}
		else
		{
			pRetVal->m_Descriptor.CPUAccessFlags = 0;
			pRetVal->m_Descriptor.Usage = D3D11_USAGE_DEFAULT;
		}

		HRESULT hRes = pDevice->CreateBuffer(&pRetVal->m_Descriptor, pInitialData, &pRetVal->m_pBuffer);
		if (hRes != S_OK)
		{
			pRetVal->m_pBuffer->Release();
			delete pRetVal;
			pRetVal = NULL;
		}


		return pRetVal;
	}
};