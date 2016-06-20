// CHighPrecisionTimerInterface.h

//system includes---
//------------------

#ifdef _WINDOWS
#include "Time/PlatformSpecific/Windows/CHighPrecisionTimerWindows.h"
#else
#include "Time/PlatformSpecific/iOS/CHighPrecisionTimeriOS.h"
#endif

#ifndef _CHighPrecisionTimerInterface_h_
#define _CHighPrecisionTimerInterface_h_

class CHighPrecisionTimerInterface
{ 
	public:
		bool Init()
		{
			return implementation.Init();
		}

		void Start()
		{
			implementation.Start();
		}

		void Stop()
		{
			implementation.Stop();
		}

		float Elapsed()
		{
			return implementation.Elapsed();
		}

private:
	CHighPrecisionTimer implementation;
};

#endif // ifndef _CHighPrecisionTimerInterface_h_