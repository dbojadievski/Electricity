#include "DirectX11RenderableInstance.h"

void
DirectX11RenderableInstance::ToInstanceData (RenderableInstanceData &data)
{
	auto transform = (this->m_CachedTransform);
	data.rowX.x = transform.m[0][0];
	data.rowX.y = transform.m[0][1];
	data.rowX.z = transform.m[0][2];
	data.rowX.w = transform.m[0][3];

	data.rowY.x = transform.m[1][0];
	data.rowY.y = transform.m[1][1];
	data.rowY.z = transform.m[1][2];
	data.rowY.w = transform.m[1][3];

	data.rowZ.x = transform.m[2][0];
	data.rowZ.y = transform.m[2][1];
	data.rowZ.z = transform.m[2][2];
	data.rowZ.w = transform.m[2][3];
	
	data.rowW.x = transform.m[3][0];
	data.rowW.y = transform.m[3][1];
	data.rowW.z = transform.m[3][2];
	data.rowW.w = transform.m[3][3];
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

MAT4 *
DirectX11RenderableInstance::GetTransform()
{
	return &this->m_Transform;
}

const MAT4 * const
DirectX11RenderableInstance::GetCachedTransform()
{
	return &this->m_CachedTransform;
}

void
DirectX11RenderableInstance::RecomputeTransform()
{
	if (this->m_pParent)
	{
		FASTMAT4 cachedParent	= XMLoadFloat4x4(&this->m_pParent->m_CachedTransform);
		FASTMAT4 local			= XMLoadFloat4x4(&this->m_Transform);

		FASTMAT4 computed		= cachedParent * local;
		XMStoreFloat4x4(&this->m_CachedTransform, computed);
	}
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
