// CHighPrecisionTimeriOS.h

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
    long timeS;
    long timeE;
};

#endif // ifndef _CHighPrecisionTimerWindows_h_