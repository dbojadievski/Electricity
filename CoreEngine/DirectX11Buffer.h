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

	ID3D11Buffer * GetRawPointer();

#pragma region Buffer factory methods.
	static DirectX11Buffer * CreateVertexBuffer(ID3D11Device * pDevice, UINT size, CORE_BOOLEAN isDynamic, CORE_BOOLEAN isStreamout, const D3D11_SUBRESOURCE_DATA * pInitialData);
	static DirectX11Buffer * CreateIndexBuffer(ID3D11Device * pDevice, UINT size, CORE_BOOLEAN isDynamic, D3D11_SUBRESOURCE_DATA * pInitialData);
	static DirectX11Buffer * CreateInstanceBuffer(ID3D11Device * pDevice, size_t instanceWidth, size_t instanceCount, CORE_BOOLEAN willCpuUpdate, const D3D11_SUBRESOURCE_DATA * pInitialData);
	static DirectX11Buffer * CreateConstantBuffer(ID3D11Device * pDevice, UINT size, CORE_BOOLEAN isDynamic, CORE_BOOLEAN willCpuUpdate, const D3D11_SUBRESOURCE_DATA * pInitialData);
	static DirectX11Buffer * CreateStructuredBuffer(ID3D11Device * pDevice, UINT count, UINT structSize, CORE_BOOLEAN canCpuWrite, CORE_BOOLEAN canGpuWrite, D3D11_SUBRESOURCE_DATA * pData);
#pragma endregion
};