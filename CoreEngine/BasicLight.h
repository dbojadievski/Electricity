#pragma once
#include "CoreTypes.h"
#include "Math.h"

class BasicLight
{
public:
	CORE_REAL m_Pad;
	FLOAT3 m_Direction;
	FLOAT4 m_ColourAmbient;
	FLOAT4 m_ColourDiffuse;
};