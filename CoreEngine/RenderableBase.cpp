#include "IRenderable.h"


RenderableBase::RenderableBase(Mesh * pMesh, size_t renderableID)
{
	this->m_pMesh = pMesh;
	this->m_ID = renderableID;
}

RenderableBase::~RenderableBase()
{
	delete this->m_pMesh;
}