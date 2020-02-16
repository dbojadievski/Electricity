#pragma once
#include "IComponent.h"
#include "Math.h"

#include "tinyxml2.h"
class TransformComponent : public IComponent
{
public:
	VEC3 m_Translation;
	VEC3 m_Rotation;
	VEC3 m_Scale;
	
	TransformComponent();
	TransformComponent(tinyxml2::XMLElement * pElement);
	~TransformComponent();

	virtual void Init();
	virtual void Update(float dT);
	virtual void Shutdown();
	virtual CORE_BOOLEAN AreMultipleInstancesAllowed() const;

	void Translate (CORE_DOUBLE x, CORE_DOUBLE y, CORE_DOUBLE z);
};

