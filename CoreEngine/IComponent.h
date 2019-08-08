#pragma once

#include <vector>
#include "CoreTypes.h"
using namespace std;

enum EComponentType
{
	COMPONENT_TYPE_UNUSED,
	COMPONENT_TYPE_TEST,
	COMPONENT_TYPE_RENDERABLE
};

class IComponent
{
public:
	EComponentType	m_Type;
	CORE_ID			m_Identifier;
	
	virtual void Init() = 0;
	virtual void Update(float dT) = 0;
	
	virtual CORE_BOOLEAN AreMultipleInstancesAllowed() const = 0;
};

typedef vector<IComponent *> ComponentList;