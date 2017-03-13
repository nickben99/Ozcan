//CHighPrecisionTimeriOS.cpp, the timer class

// ---- header files -----
#include "CHighPrecisionTimerOSX.h"
#include "CTimerOSX.h"
// -----------------------

void CHighPrecisionTimer::Start()
{
    timeS = CTimer::GetTime();
}

void CHighPrecisionTimer::Stop()
{
    timeE = CTimer::GetTime();
}

float CHighPrecisionTimer::Elapsed()
{
	return timeE - timeS;
}

bool CHighPrecisionTimer::Init()
{
	return true;
}