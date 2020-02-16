#include "TransformComponent.h"


using namespace tinyxml2;

TransformComponent::TransformComponent()
{
	this->m_Translation = VEC3(0, 0, 0);
	this->m_Rotation	= VEC3(0, 0, 0);
	this->m_Scale		= VEC3(1, 1, 1);
}

TransformComponent::TransformComponent(XMLElement * pComponent)
{
	assert(pComponent);
	if (pComponent)
	{
		XMLElement * pElemTranslation = pComponent->FirstChildElement("translation");
		if (pElemTranslation)
		{
			CORE_REAL x, y, z;
			x = pElemTranslation->FloatAttribute("x", 0);
			y = pElemTranslation->FloatAttribute("y", 0);
			z = pElemTranslation->FloatAttribute("z", 0);
			this->m_Translation = VEC3(x, y, z);
		}

		XMLElement * pElemRotation = pComponent->FirstChildElement("rotation");
		if (pElemRotation)
		{
			CORE_REAL x, y, z;
			x = pElemRotation->FloatAttribute("x", 0);
			y = pElemRotation->FloatAttribute("y", 0);
			z = pElemRotation->FloatAttribute("z", 0);
			this->m_Rotation = VEC3(x, y, z);
		}

		XMLElement * pElemScale = pComponent->FirstChildElement("scale");
		if (pElemScale)
		{
			CORE_REAL x, y, z;
			x = pElemScale->FloatAttribute("x", 0);
			y = pElemScale->FloatAttribute("y", 0);
			z = pElemScale->FloatAttribute("z", 0);
			this->m_Scale = VEC3(x, y, z);
		}

		this->m_Type = COMPONENT_TYPE_TRANSFORM;
	}
}

void
TransformComponent::Translate (CORE_DOUBLE x, CORE_DOUBLE y, CORE_DOUBLE z)
{
	this->m_Translation.x += x;
	this->m_Translation.y += y;
	this->m_Translation.z += z;
}

TransformComponent::~TransformComponent()
{
}

void
TransformComponent::Init(){}

void
TransformComponent::Update(float dT) {}

void TransformComponent::Shutdown() {}

CORE_BOOLEAN
TransformComponent::AreMultipleInstancesAllowed() const
{
	return false;
}
