// CHighPrecisionTimerWindows.h

#ifndef _CHighPrecisionTimerWindows_h_
#define _CHighPrecisionTimerWindows_h_

//system includes---
#include <windows.h> // header file for windows
//------------------

class CHighPrecisionTimer
{ 
public:
	bool Init();
	void Start();
	void Stop();
	float Elapsed();

private:
	// for timing game events
	LARGE_INTEGER timerFrequency, timeS, timeE;

	static bool sHasBeenInitialized;
};

#endif // ifndef _CHighPrecisionTimerWindows_h_