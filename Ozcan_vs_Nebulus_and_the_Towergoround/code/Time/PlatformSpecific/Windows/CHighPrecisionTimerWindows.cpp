//CHighPrecisionTimerWindows.cpp, the timer class

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
	float time = ((float)(timeE.QuadPart - timeS.QuadPart)/(float)timerFrequency.QuadPart);
	time *= 1000.0f; // convert to milliseconds
	return time;
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