#pragma once
#include "BasicLight.h"
class DirectionalLight
{
public:
	FLOAT4 m_ColourAmbient;
	FLOAT4 m_ColourDiffuse;
	CORE_REAL m_Pad;
	FLOAT3 m_Direction;

	static size_t GetSize ()
	{
		size_t parentSize	= sizeof (m_ColourAmbient);
		parentSize			+= sizeof (m_ColourDiffuse);
		parentSize			+= sizeof (m_Pad);
		parentSize			+= sizeof (m_Direction);

		return parentSize;
	}

	DirectionalLight ()
	{
		this->m_Pad				= 0;
		this->m_ColourAmbient	= VEC4 (0.0, 0.0, 0.0, 1.0);
		this->m_ColourDiffuse	= VEC4 (0.0, 0.0, 0.0, 1.0);
		this->m_Direction		= VEC3 (0.0, 0.0, 0.0);
	}
};
