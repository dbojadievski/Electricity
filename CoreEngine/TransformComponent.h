#pragma once
#include "IComponent.h"
#include "Math.h"

#include "tinyxml2.h"
class TransformComponent : public IComponent
{
private:
	VEC3 m_Translation;
	VEC3 m_Rotation;
	VEC3 m_Scale;
public:
	TransformComponent();
	TransformComponent(tinyxml2::XMLElement * pElement);
	~TransformComponent();

	virtual void Init();
	virtual void Update(float dT);
	virtual void Shutdown();
	virtual CORE_BOOLEAN AreMultipleInstancesAllowed() const;
};

