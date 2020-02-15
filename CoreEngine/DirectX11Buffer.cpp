#include "DirectX11Buffer.h"


DirectX11Buffer::DirectX11Buffer(ID3D11Device * pDevice, UINT byteWidth, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlags, D3D11_CPU_ACCESS_FLAG cpuAccessFlag)
{
	ZeroMemory(&this->m_Descriptor, sizeof(D3D11_BUFFER_DESC));
	this->m_Descriptor.Usage = usage;

	this->m_Descriptor.ByteWidth = byteWidth;
	this->m_Descriptor.BindFlags = bindFlags;
	this->m_Descriptor.CPUAccessFlags = cpuAccessFlag;

	pDevice->CreateBuffer(&this->m_Descriptor, NULL, &this->m_pBuffer);
}

ID3D11Buffer *
DirectX11Buffer::GetRawPointer()
{
	return this->m_pBuffer;
}

#pragma region Buffer factory methods.
DirectX11Buffer *
DirectX11Buffer::CreateVertexBuffer(ID3D11Device * pDevice, UINT size, CORE_BOOLEAN isDynamic, CORE_BOOLEAN isStreamout, const D3D11_SUBRESOURCE_DATA * pInitialData)
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

DirectX11Buffer * 
DirectX11Buffer::CreateIndexBuffer(ID3D11Device * pDevice, UINT size, CORE_BOOLEAN isDynamic, D3D11_SUBRESOURCE_DATA * pInitialData)
{
	assert(pDevice);
	assert(size);

	DirectX11Buffer * pRetVal = new DirectX11Buffer();

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

DirectX11Buffer *
DirectX11Buffer::CreateInstanceBuffer(ID3D11Device * pDevice, size_t instanceWidth, size_t instanceCount, CORE_BOOLEAN willCpuUpdate, const D3D11_SUBRESOURCE_DATA * pInitialData)
{
	assert(pDevice);

	DirectX11Buffer * pRetVal = new DirectX11Buffer();

	pRetVal->m_Descriptor.Usage = D3D11_USAGE_DEFAULT;
	pRetVal->m_Descriptor.ByteWidth = (UINT)(instanceWidth * instanceCount);
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

DirectX11Buffer *
DirectX11Buffer::CreateConstantBuffer(ID3D11Device * pDevice, UINT size, CORE_BOOLEAN isDynamic, CORE_BOOLEAN willCpuUpdate, const D3D11_SUBRESOURCE_DATA * pInitialData)
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

DirectX11Buffer *
DirectX11Buffer::CreateStructuredBuffer(ID3D11Device * pDevice, UINT count, UINT structSize, CORE_BOOLEAN canCpuWrite, CORE_BOOLEAN canGpuWrite, D3D11_SUBRESOURCE_DATA * pData)
{
	assert(pDevice);

	assert(count);
	assert(structSize);

	DirectX11Buffer * pRetVal = new DirectX11Buffer();
	HRESULT errCode = S_OK;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.ByteWidth = (count * structSize);
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = structSize;

	if (!canCpuWrite && !canGpuWrite)
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.CPUAccessFlags = 0;
	}
	else if (canCpuWrite && !canGpuWrite)
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (!canCpuWrite && canGpuWrite)
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
	}
	else
	{
		errCode = E_INVALIDARG;
		goto end;
	}

	errCode = pDevice->CreateBuffer(&desc, pData, &pRetVal->m_pBuffer);

end:
	assert(errCode == S_OK);
	if (errCode != S_OK)
	{
		assert(false);
		delete pRetVal;
		pRetVal = NULL;
		goto end;
	}

	return pRetVal;
}

void
DirectX11Buffer::Release ()
{
    this->m_pBuffer->Release ();
}

DirectX11Buffer::~DirectX11Buffer ()
{
	this->m_pBuffer->Release ();
}
#pragma endregion