#pragma once

#include "DirectX11Renderable.h"
#include "CoreTypes.h"
#include "Math.h"


class DirectX11Renderable;
class DirectX11RenderableInstance
{
private:
	friend DirectX11Renderable;
	CORE_ID m_ID;

	FASTMAT4 m_Transform; // NOTE(Dino): This is the object's transform relative to the parent.
	FASTMAT4 m_CachedTransform; // NOTE(Dino): This is the object's absolute transform, independent of the parent.
	DirectX11Renderable * m_pRenderable;

	DirectX11RenderableInstance * m_pParent;
	vector<DirectX11RenderableInstance *> m_Children;


	void AddChild(DirectX11RenderableInstance * pChild)
	{
		assert(pChild);
		this->m_Children.push_back(pChild);
	}
public:
	DirectX11RenderableInstance(DirectX11Renderable * pRenderable, CORE_ID id, DirectX11RenderableInstance * pParent = NULL, vector<DirectX11RenderableInstance *> * pChildren = NULL);
	~DirectX11RenderableInstance();

	const FASTMAT4 * const GetCachedTransform();
	FASTMAT4 * GetTransform();
	void RecomputeTransform();
};

