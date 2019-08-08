#pragma once

#include "CoreTypes.h"
#include "IEngineSystem.h"
#include "IEventManager.h"
#include "Renderable.h"
#include "MeshAssetDescriptor.h"
class IRenderer : IEngineSystem
{
	IEventManager * m_pEventManager;
	CORE_DOUBLE m_PrevFrameAt = 0;

public:

	virtual void Init() = 0;
	virtual void Update(CORE_DOUBLE dT) = 0;
	virtual void ShutDown() = 0;

	IRenderer();
	virtual void GetRenderablesByMesh(MeshAssetDescriptor * pMeshDesc, vector<Renderable *> * pRenderables) = 0;

};

