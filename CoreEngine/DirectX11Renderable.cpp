#include "DirectX11Renderable.h"
#include "Math.h"

void
DirectX11Renderable::Render(ID3D11DeviceContext * pDeviceContext, CORE_BOOLEAN isInstanced)
{
	assert(pDeviceContext);

	this->ActivateBuffers(pDeviceContext);
	pDeviceContext->IASetPrimitiveTopology(this->m_Topology);
	size_t indiceCount		= this->m_pMesh->GetIndiceCount();
	size_t vertexCount		= this->m_pMesh->GetVertexCount();
	size_t instanceCount	= this->m_pInstances.size ();

	if (isInstanced)
		pDeviceContext->DrawIndexedInstanced ((UINT)indiceCount, instanceCount, 0, 0, 0);
	else
		pDeviceContext->DrawIndexed ((UINT)indiceCount, 0, 0);
}

void
DirectX11Renderable::Buffer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	assert(pDevice);
	assert(pDeviceContext);

	{
		if (this->m_pVertexBuffer)
			delete this->m_pVertexBuffer;

		size_t numVertices = this->m_pMesh->GetVertexCount();
		Vertex * pVertices = this->m_pMesh->GetVerticesRaw();
		size_t vertexArraySize = this->m_pMesh->GetVertexArraySize();

		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = pVertices;
		this->m_pVertexBuffer = DirectX11Buffer::CreateVertexBuffer(pDevice, (UINT)( sizeof(Vertex) * numVertices), false, false, &vertexData);
	}

	{
		if (this->m_pIndexBuffer)
			delete this->m_pIndexBuffer;

		size_t numIndices = this->m_pMesh->GetIndiceCount();
		unsigned long int * pIndices = this->m_pMesh->GetIndicesRaw();
		D3D11_SUBRESOURCE_DATA buffData;
		buffData.pSysMem = pIndices;
		this->m_pIndexBuffer = DirectX11Buffer::CreateIndexBuffer(pDevice, (UINT)(sizeof(unsigned long int) * numIndices), false, &buffData);
	}

	{
		if (this->m_pInstanceBuffer)
			delete this->m_pInstanceBuffer;

		size_t numInstances		= this->m_pInstances.size ();
		
		if (this->m_pInstanceBuff)
			delete [] this->m_pInstanceBuff;
		this->m_pInstanceBuff	= (RenderableInstanceData *)malloc (RenderableInstanceData::GetInstanceSize() * numInstances);
		ZeroMemory (m_pInstanceBuff, RenderableInstanceData::GetInstanceSize () * numInstances);
		for (size_t idx			= 0; idx < numInstances; idx++)
		{
			auto pInstance		= this->m_pInstances[idx];
			RenderableInstanceData data;
			pInstance->ToInstanceData(data);
			memcpy (&this->m_pInstanceBuff[idx], (const void *) &data, sizeof(data));
		}
		D3D11_SUBRESOURCE_DATA _instanceData;
		_instanceData.pSysMem	= m_pInstanceBuff;
		this->m_pInstanceBuffer = DirectX11Buffer::CreateInstanceBuffer (pDevice, RenderableInstanceData::GetInstanceSize(), numInstances, false, &_instanceData);
	}
}

void 
DirectX11Renderable::ActivateBuffers(ID3D11DeviceContext * pDeviceContext)
{
	assert(pDeviceContext);

	UINT offsets[2] = { 0 };
	UINT strides[2] = 
	{ 
		this->m_pMesh->GetVertexSize(), 
		RenderableInstanceData::GetInstanceSize()
	};

	ID3D11Buffer* buffs[2] =
	{
		this->m_pVertexBuffer->GetRawPointer(),
		this->m_pInstanceBuffer->GetRawPointer()
	};

	ID3D11Buffer * pVertexBuffer	= this->m_pVertexBuffer->GetRawPointer();
	ID3D11Buffer * pIndexBuffer		= this->m_pIndexBuffer->GetRawPointer();
	ID3D11Buffer * pInstanceBuffer = this->m_pInstanceBuffer->GetRawPointer ();

	pDeviceContext->IASetVertexBuffers (0, 2, buffs, strides, offsets);
	pDeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void
DirectX11Renderable::DeactivateBuffers ()
{
	this->m_pIndexBuffer->Release();
	this->m_pIndexBuffer	= NULL;

	this->m_pInstanceBuffer->Release ();
	this->m_pInstanceBuffer = NULL;
	
	delete[] this->m_pInstanceBuff;
	this->m_pInstanceBuff	= NULL;

	this->m_pVertexBuffer->Release ();
	this->m_pVertexBuffer	= NULL;
}

DirectX11Renderable::DirectX11Renderable(Mesh * pMesh, DirectX11Texture2D * pTexture, DirectX11Shader * pShader, CORE_BOOLEAN isTransparent, D3D11_PRIMITIVE_TOPOLOGY topology) : Renderable(pMesh->GetIdentifier(), pTexture->m_Identifier, pShader->GetIdentifier(), isTransparent)
{
	assert(pMesh);
	assert(pShader);
	assert(pTexture);
	assert(topology);

	this->m_pShader				= pShader;
	this->m_pTexture			= pTexture;

	this->m_pVertexBuffer		= NULL;
	this->m_pIndexBuffer		= NULL;
	
	this->m_pInstanceBuffer		= NULL;
	this->m_pInstanceBuff		= NULL;
	
	this->m_Topology			= topology;
	this->m_pMesh				= pMesh;
}

DirectX11Renderable::~DirectX11Renderable()
{
	if (this->m_pVertexBuffer)
		delete this->m_pVertexBuffer;

	if (this->m_pIndexBuffer)
		delete this->m_pIndexBuffer;

	if (this->m_pInstanceBuffer)
		delete this->m_pInstanceBuffer;

	if (this->m_pTexture)
		delete this->m_pTexture;

	if (this->m_pInstanceBuff)
		delete[] this->m_pInstanceBuff;

	size_t numInstances = this->m_pInstances.size();
	for (size_t i = 0; i < numInstances; i++)
		delete this->m_pInstances[i];


	this->m_pInstances.clear();
}

DirectX11RenderableInstance *
DirectX11Renderable::Instantiate(CORE_ID id, FASTMAT4 transform, Material * pMaterial, DirectX11RenderableInstance * pParent)
{
	assert(id);

	DirectX11RenderableInstance * pInstance = NULL;
	
	pInstance = new DirectX11RenderableInstance(this, id, pMaterial, pParent, NULL);

	if (pParent)
	{
		pInstance->m_pParent = pParent;
		pParent->AddChild(pInstance);
	}
	XMStoreFloat4x4(&pInstance->m_Transform, transform);
	pInstance->RecomputeTransform();
	this->m_pInstances.push_back(pInstance);

	return pInstance;
}

CORE_BOOLEAN
DirectX11Renderable::DeInstantiate (CORE_ID id)
{
	CORE_BOOLEAN result = false;

	for (auto it	= this->m_pInstances.begin (); it != this->m_pInstances.end (); it++)
	{
		auto val	= (*it);
		if (val->m_ID == id)
		{
			result	= true;
			this->m_pInstances.erase (it);
			break;
		}

	}

	return result;
}

DWORD
DirectX11Renderable::GetInstanceCount() const
{
	return (DWORD) this->m_pInstances.size();
}

DirectX11RenderableInstanceIterator
DirectX11Renderable::GetInstances()
{
	DirectX11RenderableInstanceIterator instanceIterator = this->m_pInstances.begin();
	return instanceIterator;
}

DirectX11RenderableInstanceIterator 
DirectX11Renderable::GetInstancesEnd()
{
	DirectX11RenderableInstanceIterator instanceIterator = this->m_pInstances.end();
	return instanceIterator;
}

DirectX11RenderableInstance *
DirectX11Renderable::GetInstance (CORE_ID renderableComponentId)
{
	DirectX11RenderableInstance * pRetVal = NULL;
	for (auto it = this->m_pInstances.begin (); it != this->m_pInstances.end (); it++)
	{
		auto val = (*it);
		if (val->m_ID == renderableComponentId)
		{
			pRetVal = val;
			break;
		}

	}

	return pRetVal;
}

const DirectX11Shader * 
DirectX11Renderable::GetShader() const
{
	return (const DirectX11Shader *) this->m_pShader;
}

const DirectX11Texture2D *
DirectX11Renderable::GetTexture() const
{
	return (const DirectX11Texture2D *)this->m_pTexture;
}

const Mesh *
DirectX11Renderable::GetMesh () const
{
	return (const Mesh * ) this->m_pMesh;
}