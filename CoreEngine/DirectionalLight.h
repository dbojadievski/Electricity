#pragma once
#include "BasicLight.h"
class DirectionalLight : public BasicLight
{
public:
	CORE_REAL m_Pad;
	FLOAT3 m_Direction;
};
