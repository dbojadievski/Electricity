#include "DirectX11IndexBuffer.h"

DirectX11IndexBuffer::DirectX11IndexBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, UINT byteWidth, unsigned int * pData)
{
	assert(pDevice);
	assert(pDeviceContext);
	assert(byteWidth > 0);
	assert(pData);

	this->m_Descriptor.Usage = D3D11_USAGE_DEFAULT;
	this->m_Descriptor.ByteWidth = byteWidth;
	this->m_Descriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
	this->m_Descriptor.CPUAccessFlags = 0;
	this->m_Descriptor.MiscFlags = 0;

	this->m_InitData.pSysMem = pData;
	this->m_InitData.SysMemPitch = 0;
	this->m_InitData.SysMemSlicePitch = 0;

	pDevice->CreateBuffer(&this->m_Descriptor, &this->m_InitData, &this->m_pBuffer);
	pDeviceContext->IASetIndexBuffer(this->m_pBuffer, DXGI_FORMAT_R32_UINT, 0);
}

ID3D11Buffer * DirectX11IndexBuffer::GetRawPointer()
{
	return this->m_pBuffer;
}