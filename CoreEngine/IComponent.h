#pragma once

enum EComponentType
{
	UNUSED
};

class IComponent
{
public:
	EComponentType m_Type;
	CORE_ID m_Identifier;
	virtual void Init() = 0;
	virtual void Update(float dT) = 0;
};