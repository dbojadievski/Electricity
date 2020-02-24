#pragma once
#include "BasicLight.h"
struct HemisphericLight
{
	FLOAT3	m_AmbientDown;
	FLOAT3	m_AmbientUp;
	FLOAT2	m_Pad;

	static size_t GetSize ();
	HemisphericLight ();
};
