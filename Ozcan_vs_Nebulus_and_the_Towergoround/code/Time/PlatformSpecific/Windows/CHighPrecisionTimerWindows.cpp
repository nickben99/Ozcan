//CHighPrecisionTimerWindows.cpp, the timer class

#ifdef _WINDOWS

// ---- header files -----
#include "CHighPrecisionTimerWindows.h"
// -----------------------

bool CHighPrecisionTimer::sHasBeenInitialized = false;

void CHighPrecisionTimer::Start()
{
	QueryPerformanceCounter(&timeS); // for recording frame lengths
}

void CHighPrecisionTimer::Stop()
{
	QueryPerformanceCounter(&timeE); // for recording frame lengths
}

float CHighPrecisionTimer::Elapsed()
{
	return ((float)(timeE.QuadPart - timeS.QuadPart)/(float)timerFrequency.QuadPart);
}

bool CHighPrecisionTimer::Init()
{
	if (!sHasBeenInitialized)
	{
		// get the number of ticks per second and save in timerFrequency
		sHasBeenInitialized = QueryPerformanceFrequency(&timerFrequency) ? true : false;
	}
	return sHasBeenInitialized;
}

#endif