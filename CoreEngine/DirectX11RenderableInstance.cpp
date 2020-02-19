#include "DirectX11RenderableInstance.h"

void
DirectX11RenderableInstance::ToInstanceData (RenderableInstanceData &data)
{
	auto transform = (this->m_CachedTransform);
	data.m_rowX = transform.r[0];
	data.m_rowY = transform.r[1];
	data.m_rowZ = transform.r[2];
	data.m_rowW = transform.r[3];
}

DirectX11RenderableInstance::DirectX11RenderableInstance(DirectX11Renderable * pRenderable, CORE_ID id, DirectX11RenderableInstance * pParent, vector<DirectX11RenderableInstance *> * pChildren)
{
	this->m_ID = id;

	this->m_pRenderable = pRenderable;

	this->m_pParent = pParent;

	if (pChildren)
	{
		auto childIt = pChildren->begin();
		while (childIt != pChildren->end())
		{
			DirectX11RenderableInstance * pChild = *childIt;
			this->m_Children.push_back(pChild);

			++childIt;
		}
	}
}

DirectX11RenderableInstance::~DirectX11RenderableInstance()
{
	this->m_pRenderable = NULL;
	auto childIt = this->m_Children.begin();
	while (childIt != this->m_Children.end())
	{
		DirectX11RenderableInstance * pChild = *childIt;
		delete pChild;

		++childIt;
	}
}

FASTMAT4 *
DirectX11RenderableInstance::GetTransform()
{
	return &this->m_Transform;
}

const FASTMAT4 * const
DirectX11RenderableInstance::GetCachedTransform()
{
	return &this->m_CachedTransform;
}

void
DirectX11RenderableInstance::RecomputeTransform()
{
	if (this->m_pParent)
		this->m_CachedTransform = (this->m_pParent->m_CachedTransform * this->m_Transform);
	else
		this->m_CachedTransform = this->m_Transform;

	if (this->m_Children.size())
	{
		auto childIt = this->m_Children.begin();
		while (childIt != this->m_Children.end())
		{
			DirectX11RenderableInstance * pChild = *childIt;
			pChild->RecomputeTransform();
		}
	}
}

