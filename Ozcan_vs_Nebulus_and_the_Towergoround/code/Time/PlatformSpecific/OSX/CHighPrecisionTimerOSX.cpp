//CHighPrecisionTimeriOS.cpp, the timer class

#ifdef OSX

// ---- header files -----
#include "CHighPrecisionTimerOSX.h"
// -----------------------

void CHighPrecisionTimer::Start()
{
}

void CHighPrecisionTimer::Stop()
{
}

float CHighPrecisionTimer::Elapsed()
{
	return 0.0f;
}

bool CHighPrecisionTimer::Init()
{
	return true;
}

#endif // #ifdef OSX