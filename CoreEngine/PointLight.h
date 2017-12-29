#pragma once
#include "BasicLight.h"

class PointLight : BasicLight
{
public:
	FLOAT3 m_Position;
	CORE_REAL m_Range;
	FLOAT3 m_Attentuation;
	CORE_REAL m_padding2;
};