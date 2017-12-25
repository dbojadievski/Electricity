#include "DirectX11Renderable.h"
#include "Math.h"

void
DirectX11Renderable::Render(ID3D11DeviceContext * pDeviceContext)
{
	assert(pDeviceContext);

	this->ActivateBuffers(pDeviceContext);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	size_t indiceCount = this->m_pMesh->GetIndiceCount();
	size_t vertexCount = this->m_pMesh->GetVertexCount();

	pDeviceContext->DrawIndexed(indiceCount, 0, 0);
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

		this->m_pVertexBuffer = new DirectX11Buffer(pDevice, sizeof(Vertex) * numVertices);
		this->m_pVertexBuffer->LoadToVideoMemory(pDeviceContext, pVertices, vertexArraySize);
	}

	{
		if (this->m_pIndexBuffer)
			delete this->m_pIndexBuffer;

		size_t numIndices = this->m_pMesh->GetIndiceCount();
		unsigned int * pIndices = this->m_pMesh->GetIndicesRaw();
		this->m_pIndexBuffer = new DirectX11IndexBuffer(pDevice, pDeviceContext, sizeof(unsigned int) * numIndices, pIndices);
	}
}

void 
DirectX11Renderable::ActivateBuffers(ID3D11DeviceContext * pDeviceContext)
{
	assert(pDeviceContext);

	UINT offset = 0;
	UINT stride = this->m_pMesh->GetVertexSize();

	ID3D11Buffer * pVertexBuffer = this->m_pVertexBuffer->GetRawPointer();

	pDeviceContext->IASetVertexBuffers(0, 1, &	pVertexBuffer, &stride, &offset);
}

DirectX11Renderable::DirectX11Renderable(Mesh * pMesh, DirectX11Texture2D * pTexture, DirectX11Shader * pShader, CORE_BOOLEAN isTransparent)
{
	assert(pMesh);
	assert(pShader);
	assert(pTexture);

	this->m_pMesh = pMesh;
	this->m_pShader = pShader;
	this->m_pTexture = pTexture;

	this->m_pVertexBuffer = NULL;
	this->m_pIndexBuffer = NULL;

	this->m_IsTransparent = isTransparent;
}

DirectX11Renderable::~DirectX11Renderable()
{
	if (this->m_pVertexBuffer)
		delete this->m_pVertexBuffer;

	if (this->m_pIndexBuffer)
		delete this->m_pIndexBuffer;

	if (this->m_pTexture)
		delete this->m_pTexture;

	size_t numInstances = this->m_pInstances.size();
	for (size_t i = 0; i < numInstances; i++)
		delete this->m_pInstances[i];


	this->m_pInstances.clear();
}

DirectX11RenderableInstance *
DirectX11Renderable::Instantiate(CORE_ID id, FASTMAT4 transform, DirectX11RenderableInstance * pParent)
{
	assert(id);

	DirectX11RenderableInstance * pInstance = NULL;
	
	pInstance = new DirectX11RenderableInstance(this, id, pParent, NULL);

	if (pParent)
	{
		pInstance->m_pParent = pParent;
		pParent->AddChild(pInstance);
	}
	pInstance->m_Transform = transform;
	pInstance->RecomputeTransform();
	this->m_pInstances.push_back(pInstance);

	return pInstance;
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


const DirectX11Shader * 
DirectX11Renderable::GetShader()
{
	return (const DirectX11Shader *) this->m_pShader;
}

const DirectX11Texture2D *
DirectX11Renderable::GetTexture()
{
	return (const DirectX11Texture2D *)this->m_pTexture;
}

CORE_BOOLEAN
DirectX11Renderable::IsTransparent()
{
	return this->m_IsTransparent;
}