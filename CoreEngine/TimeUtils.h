#pragma once

#include "CoreTypes.h"
class Timer
{
private:
	float m_TicksPerSecond;
	__int64 m_FirstTickAt;

public:

	void Init();
	void Start();
	CORE_DOUBLE GetTime();
};