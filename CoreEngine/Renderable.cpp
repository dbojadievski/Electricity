#include "Renderable.h"



Renderable::Renderable(CORE_ID meshID, CORE_ID textureID, CORE_ID shaderID, CORE_BOOLEAN isTransparent)
{
	this->m_MeshID = meshID;
	this->m_ShaderID = shaderID;
	this->m_TextureID = textureID;
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
}
