#include "DirectX11RenderSet.h"

DirectX11RenderSet::DirectX11RenderSet()
{
}

CORE_BOOLEAN
DirectX11RenderSet::Insert(DirectX11Renderable * pRenderable)
{
	assert(pRenderable);
	
	CORE_BOOLEAN wasInserted = false;

	CORE_BOOLEAN isTransparent = pRenderable->IsTransparent();
	DirectX11RenderableMap * pMap = isTransparent ? &this->m_TransparentRenderables : &this->m_OpaqueRenderables;
	assert(pMap);

	DirectX11Shader * pShader = (DirectX11Shader *) pRenderable->GetShader();
	assert(pShader);
	CORE_ID shaderId = pShader->GetIdentifier();
	auto forShaderIter = pMap->find(shaderId);
	DirectX11RenderablesPerTextureSet * pPerTextureSet = NULL;
	if (forShaderIter == pMap->end())
	{
		/* It would seem no renderables currently use this shader. */
		pPerTextureSet = new DirectX11RenderablesPerTextureSet();
		auto pair = make_pair(shaderId, pPerTextureSet);
		pMap->insert(pair);
	}
	else
		pPerTextureSet = forShaderIter->second;

	assert(pPerTextureSet);
	CORE_ID textureId = pRenderable->GetTexture()->m_Identifier;
	DirectX11RenderableList * pRenderableList = NULL;
	auto forTexIter = pPerTextureSet->find(textureId);
	if (forTexIter == pPerTextureSet->end())
	{
		pRenderableList = new DirectX11RenderableList();
		auto pair = make_pair(textureId, pRenderableList);
		pPerTextureSet->insert(pair);
	}
	else
		pRenderableList = forTexIter->second;

	assert(pRenderableList);
	CORE_BOOLEAN renderableAlreadyExists = (find(pRenderableList->begin(), pRenderableList->end(), pRenderable) != pRenderableList->end());
	if (!renderableAlreadyExists)
	{
		pRenderableList->push_back(pRenderable);
		wasInserted = true;
	}

	return wasInserted;
}

CORE_BOOLEAN
DirectX11RenderSet::Remove(DirectX11Renderable * pRenderable)
{
	assert(pRenderable);
	CORE_BOOLEAN wasRemoved = false;

	CORE_BOOLEAN isTransparent = pRenderable->IsTransparent();
	DirectX11RenderableMap * pMap = isTransparent ? &this->m_TransparentRenderables : &this->m_OpaqueRenderables;
	assert(pMap);

	DirectX11Shader * pShader = (DirectX11Shader *)pRenderable->GetShader();
	assert(pShader);
	CORE_ID shaderId = pShader->GetIdentifier();
	auto forShaderIter = pMap->find(shaderId);
	if (forShaderIter != pMap->end())
	{
		DirectX11RenderablesPerTextureSet * pPerTextureSet = forShaderIter->second;
		CORE_ID textureId = pRenderable->GetTexture()->m_Identifier;
		auto forTexIter = pPerTextureSet->find(textureId);
		if (forTexIter != pPerTextureSet->end())
		{
			DirectX11RenderableList * pRenderableList = forTexIter->second;
			auto posIter = find(pRenderableList->begin(), pRenderableList->end(), pRenderable);
			if (posIter != pRenderableList->end())
			{
				pRenderableList->erase(posIter);
				wasRemoved = true;
			}
			if (wasRemoved && !pRenderableList->size ())
				pPerTextureSet->erase (textureId);
		}
	}

	return wasRemoved;

}

void
DirectX11RenderSet::Clear()
{
	ClearSetInternal(&this->m_OpaqueRenderables);
	ClearSetInternal(&this->m_TransparentRenderables);
}

void 
DirectX11RenderSet::ClearSetInternal(DirectX11RenderableMap * pToClear)
{
	auto iter = pToClear->begin();
	while (iter != pToClear->end())
	{
		DirectX11RenderablesPerTextureSet pPerTexSet = *iter->second;
		auto iterPerTexSet = pPerTexSet.begin();
		while (iterPerTexSet != pPerTexSet.end())
		{
			DirectX11RenderableList * pVec = iterPerTexSet->second;
			delete pVec;
			iterPerTexSet++;
		}
		pPerTexSet.clear();
		iter++;
	}
	pToClear->clear();
}

