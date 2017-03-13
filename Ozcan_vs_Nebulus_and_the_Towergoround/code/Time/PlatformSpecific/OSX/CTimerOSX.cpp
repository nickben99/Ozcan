//CTimer.cpp, the timer class

// ---- header files -----
#include "CTimerOSX.h"
#include <sys/time.h>
// -----------------------

//constructor
CTimer::CTimer() 
{
} 

// destructor
CTimer::~CTimer()
{
}

unsigned int CTimer::time()
{
    return (unsigned int)GetTime();
}

long CTimer::GetTime()
{
    timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec * 1000) + (time.tv_usec / 1000);
}