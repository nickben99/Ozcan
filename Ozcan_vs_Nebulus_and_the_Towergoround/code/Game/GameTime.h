//GameTime.h, header file for GameTime.cpp

//system includes---
//------------------

#ifndef _GameTime_h_
#define _GameTime_h_


class GameTime
{ 
public:
	static GameTime& Instance();
	~GameTime();

	void Reset();

	bool IsPaused() { return paused;};
	void Pause(bool doPause) { paused = doPause; };
	
	int GetTimeDeltaMilli();
	float GetTimeDeltaSeconds();	
	
	void UseFixedTime();
	int GetCurrentFixedTimeDeltaMilli();
	float GetCurrentFixedTimeDeltaSeconds();

	int GetFixedTimeDeltaMilli();
	float GetFixedTimeDeltaSeconds();

	float GetElapsed();

	void Update();

	void ProcessKeyInput();

private:
	GameTime();

	enum eTimeMode
	{
		eTMNormal = 0, 
		eTMHalf, 
		eTMQuarter, 
		eTMEighth, 
		eTMSixteenth
	};

	void GetFixedTimeDelta(eTimeMode mode, unsigned int& milliSeconds, float& seconds);
	float ConvertMilliSecondsToSeconds(int milliSeconds);	

private:
	unsigned int timeDifference;
	float timeDifferenceAsPerCentOfSec;	

	eTimeMode mTimeMode;
	float elapsed;
	bool paused;
};

#endif // ifndef _GameTime_h_