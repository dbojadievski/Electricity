#pragma once


#include "DirectX11Renderable.h"

#include <map>
#include <assert.h>
using namespace std;


typedef vector<DirectX11Renderable *> DirectX11RenderableList;
typedef map<CORE_ID, DirectX11RenderableList *> DirectX11RenderablesPerTextureSet;
typedef map<CORE_ID, DirectX11RenderablesPerTextureSet *> DirectX11RenderableMap;


class DirectX11RenderSet
{
	friend class DirectX11Renderer;
private:
	DirectX11RenderableMap m_TransparentRenderables;
	DirectX11RenderableMap m_OpaqueRenderables;
	
	void ClearSetInternal(DirectX11RenderableMap * pToClear);
public: 
	DirectX11RenderSet();
	void Clear();
	CORE_BOOLEAN Insert(DirectX11Renderable * pRenderable);
	CORE_BOOLEAN Remove(DirectX11Renderable * pRenderable);
};