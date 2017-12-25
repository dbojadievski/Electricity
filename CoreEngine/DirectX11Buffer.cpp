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

void 
DirectX11Buffer::LoadToVideoMemory(ID3D11DeviceContext * pDeviceContext, void * pData, size_t dataSize, UINT subresource, D3D11_MAP mapType, UINT mapFlags)
{
	pDeviceContext->Map(this->m_pBuffer, subresource, mapType, mapFlags, &this->m_Subresource);
	memcpy(this->m_Subresource.pData, pData, dataSize);
	pDeviceContext->Unmap(this->m_pBuffer, subresource);
}

ID3D11Buffer *
DirectX11Buffer::GetRawPointer()
{
	return this->m_pBuffer;
}