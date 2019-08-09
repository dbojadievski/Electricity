#include "IComponent.h"

EComponentType
IComponent::StringToType(const string & type)
{
	EComponentType retVal = COMPONENT_TYPE_UNUSED;
	if (type == "transform")
		retVal = COMPONENT_TYPE_TRANSFORM;
	else if (type == "renderable")
		retVal = COMPONENT_TYPE_RENDERABLE;

	return retVal;
}