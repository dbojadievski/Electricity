#pragma once
#include "CoreTypes.h"
#include <d3d11.h>
#include <assert.h>

class DirectX11Buffer
{
private:
	DirectX11Buffer(){}
	ID3D11Buffer * m_pBuffer;
	D3D11_BUFFER_DESC m_Descriptor;
	D3D11_MAPPED_SUBRESOURCE m_Subresource;

public:
	DirectX11Buffer(ID3D11Device * pDevice, UINT byteWidth, D3D11_USAGE usage = D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG bindFlags = D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_FLAG cpuAccessFlag = D3D11_CPU_ACCESS_WRITE);
	void LoadToVideoMemory(ID3D11DeviceContext * pDeviceContext, void * pData, size_t dataSize, UINT subresource = NULL, D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD, UINT mapFlags = NULL);

	ID3D11Buffer * GetRawPointer();

	static DirectX11Buffer * 
	CreateVertexBuffer(ID3D11Device * pDevice, UINT size, CORE_BOOLEAN isDynamic, CORE_BOOLEAN isStreamout, const D3D11_SUBRESOURCE_DATA * pInitialData)
	{
		DirectX11Buffer * pRetVal = new DirectX11Buffer();

		pRetVal->m_Descriptor.ByteWidth = size;
		pRetVal->m_Descriptor.MiscFlags = 0;
		pRetVal->m_Descriptor.StructureByteStride = 0;

		if (isStreamout)
			pRetVal->m_Descriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
		else
			pRetVal->m_Descriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		if (isDynamic)
		{
			pRetVal->m_Descriptor.Usage = D3D11_USAGE_DYNAMIC;
			pRetVal->m_Descriptor.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		}
		else
		{
			pRetVal->m_Descriptor.Usage = D3D11_USAGE_IMMUTABLE;
			pRetVal->m_Descriptor.CPUAccessFlags = 0;
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

	static DirectX11Buffer *
	CreateConstantBuffer(ID3D11Device * pDevice, UINT size, CORE_BOOLEAN isDynamic, CORE_BOOLEAN willCpuUpdate, const D3D11_SUBRESOURCE_DATA * pInitialData)
	{
		DirectX11Buffer * pRetVal = new DirectX11Buffer();

		pRetVal->m_Descriptor.ByteWidth = size;
		pRetVal->m_Descriptor.MiscFlags = 0;
		pRetVal->m_Descriptor.StructureByteStride = 0;
		pRetVal->m_Descriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;


		if (isDynamic && willCpuUpdate)
		{
			pRetVal->m_Descriptor.Usage = D3D11_USAGE_DYNAMIC;
			pRetVal->m_Descriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else if (isDynamic && !willCpuUpdate)
		{
			pRetVal->m_Descriptor.Usage = D3D11_USAGE_DEFAULT;
			pRetVal->m_Descriptor.CPUAccessFlags = 0;
		}
		else
		{
			pRetVal->m_Descriptor.Usage = D3D11_USAGE_IMMUTABLE;
			pRetVal->m_Descriptor.CPUAccessFlags = 0;
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

	static DirectX11Buffer *
		CreateInstanceBuffer(ID3D11Device * pDevice, size_t instanceWidth, size_t instanceCount, CORE_BOOLEAN willCpuUpdate, const D3D11_SUBRESOURCE_DATA * pInitialData)
	{
		assert(pDevice);

		DirectX11Buffer * pRetVal = new DirectX11Buffer();

		pRetVal->m_Descriptor.Usage = D3D11_USAGE_DEFAULT;
		pRetVal->m_Descriptor.ByteWidth = instanceWidth * instanceCount;
		pRetVal->m_Descriptor.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		pRetVal->m_Descriptor.CPUAccessFlags = willCpuUpdate;
		pRetVal->m_Descriptor.MiscFlags = 0;
		pRetVal->m_Descriptor.StructureByteStride = 0;

		//already initted data.
		HRESULT result = pDevice->CreateBuffer(&pRetVal->m_Descriptor, pInitialData, &pRetVal->m_pBuffer);
		assert(result);
		if (!result)
		{
			delete pRetVal;
			pRetVal = NULL;
		}

		return pRetVal;
	}
};