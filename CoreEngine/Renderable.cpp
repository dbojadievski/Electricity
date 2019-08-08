#include "Renderable.h"



Renderable::Renderable(Mesh * pMesh, CORE_BOOLEAN isTransparent)
{
	assert(pMesh);
	this->m_pMesh = pMesh;
	this->m_IsTransparent = isTransparent;
}

CORE_BOOLEAN
Renderable::IsTransparent() const
{
	return this->m_IsTransparent;
}

Renderable::~Renderable()
{
	this->m_IsTransparent	= false;
	this->m_RenderableID	= 0;

	delete this->m_pMesh;
}

Mesh *
Renderable::GetMesh() const
{
	return this->m_pMesh;
}
