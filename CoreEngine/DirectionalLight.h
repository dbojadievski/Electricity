#pragma once
#include "BasicLight.h"
class DirectionalLight
{
public:
	FLOAT3 m_Direction;
	FLOAT3 m_Colour;
	FLOAT2	m_Pad;

	static size_t GetSize ()
	{
		size_t parentSize = 0;
		parentSize			+= sizeof (m_Pad);
		parentSize			+= sizeof (m_Direction);
		parentSize			+= sizeof (m_Colour);
		return parentSize;
	}

	DirectionalLight ()
	{
		this->m_Pad				= VEC2(0, 0);
		this->m_Colour			= VEC3 (0.0, 0.0, 0.0);
		this->m_Direction		= VEC3 (0.0, 0.0, 0.0);
	}
};
