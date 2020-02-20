#pragma once
#include "CoreTypes.h"
#include "Math.h"

class BasicLight
{
public:
	FLOAT4 m_ColourAmbient;
	FLOAT4 m_ColourDiffuse;

	static size_t GetSize ()
	{
		size_t result = 0;
		result += sizeof(m_ColourAmbient);
		result += sizeof (m_ColourDiffuse);
		return result;
	}
};

