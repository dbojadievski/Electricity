#include "RenderableComponent.h"



RenderableComponent::RenderableComponent(Entity * pOwner, Renderable * pRenderable)
{
	assert(pOwner);
	assert(pRenderable);

	this->m_Type			= COMPONENT_TYPE_RENDERABLE;
	this->m_pOwner			= pOwner;
	this->m_pRenderable		= pRenderable;
}

Entity *
RenderableComponent::GetOwner() const
{
	return this->m_pOwner;
}

Renderable *
RenderableComponent::GetRenderable() const
{
	return this->m_pRenderable;
}

CORE_BOOLEAN
RenderableComponent::AreMultipleInstancesAllowed() const
{
	return false;
}

void
RenderableComponent::Init()
{}

void
RenderableComponent::Update(float dT)
{

}