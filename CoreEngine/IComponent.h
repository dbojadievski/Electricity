#pragma once

#include <vector>
#include "CoreTypes.h"
using namespace std;

enum EComponentType
{
	COMPONENT_TYPE_UNUSED,
	COMPONENT_TYPE_TEST,
	COMPONENT_TYPE_TRANSFORM,
	COMPONENT_TYPE_RENDERABLE
};

class IComponent
{
	
public:
	EComponentType	m_Type;
	CORE_ID			m_Identifier;
	//Entity *		m_pOwner;

	virtual void Init() = 0;
	virtual void Update(float dT) = 0;
	
	virtual CORE_BOOLEAN AreMultipleInstancesAllowed() const = 0;

	static EComponentType StringToType(const string & type);
};

typedef vector<IComponent *> ComponentList;