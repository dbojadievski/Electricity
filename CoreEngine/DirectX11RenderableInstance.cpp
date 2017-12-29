#include "DirectX11RenderableInstance.h"

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
	delete this->m_pRenderable;
	auto childIt = this->m_Children.begin();
	while (childIt != this->m_Children.end())
	{
		DirectX11RenderableInstance * pChild = *childIt;
		delete pChild;

		++childIt;
	}
}

const FASTMAT4 * const
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

