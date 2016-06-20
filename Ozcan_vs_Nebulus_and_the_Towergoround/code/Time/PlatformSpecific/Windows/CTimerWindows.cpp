//CTimer.cpp, the timer class

#ifdef _WINDOWS

// ---- header files -----
#include "CTimerWindows.h"
#include <windows.h>
// -----------------------

//constructor
CTimer::CTimer() 
{ 
	timeBeginPeriod(1); // set resolution
} 

// destructor
CTimer::~CTimer()
{
	timeEndPeriod(1); // set resolution
}

unsigned int CTimer::time()
{
	return(timeGetTime()); // return time
}

#endif // #ifdef _WINDOWS