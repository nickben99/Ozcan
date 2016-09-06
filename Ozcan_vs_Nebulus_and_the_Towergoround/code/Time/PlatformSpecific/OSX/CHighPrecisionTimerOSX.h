// CHighPrecisionTimeriOS.h

#ifdef OSX

#ifndef _CHighPrecisionTimer_h_
#define _CHighPrecisionTimer_h_

//system includes---
//------------------

class CHighPrecisionTimer
{ 
public:
	bool Init();
	void Start();
	void Stop();
	float Elapsed();

private:
};

#endif // ifndef _CHighPrecisionTimerWindows_h_

#endif // #ifdef OSX