#pragma once
#include "IComponent.h"
#include "Renderable.h"
#include "GameObject.h"
class RenderableComponent : public IComponent
{
protected:
	Entity *	m_pOwner;
	Renderable * m_pRenderable;
public:
	RenderableComponent(Entity * pOwner, Renderable * pRenderable);

	Entity * GetOwner() const;
	Renderable * GetRenderable() const;

	virtual void Init();
	virtual void Update(float dT);
	virtual CORE_BOOLEAN AreMultipleInstancesAllowed() const;
};

