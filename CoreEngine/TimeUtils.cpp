#include "TimeUtils.h"
#include <tchar.h>
#include <Windows.h>


void 
Timer::Init()
{
	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);

	this->m_TicksPerSecond = double(frequencyCount.QuadPart);
	this->m_FirstTickAt = frequencyCount.QuadPart;
}

CORE_DOUBLE
Timer::GetTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	CORE_DOUBLE tick = double(currentTime.QuadPart - this->m_FirstTickAt) / this->m_TicksPerSecond;

	return tick;
}