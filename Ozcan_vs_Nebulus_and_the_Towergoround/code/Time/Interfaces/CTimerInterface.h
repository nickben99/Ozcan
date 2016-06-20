// CTimerInterface.h

//system includes---
//------------------

#ifdef _WINDOWS
#include "Time/PlatformSpecific/Windows/CTimerWindows.h"
#else
#include "Time/PlatformSpecific/iOS/CTimeriOS.h"
#endif

#ifndef _CTimerInterface_h_
#define _CTimerInterface_h_

class CTimerInterface
{ 
	public:
		unsigned int time()
		{
			return implementation.time();
		}

private:
	CTimer implementation;
};

#endif // ifndef _CTimerInterface_h_