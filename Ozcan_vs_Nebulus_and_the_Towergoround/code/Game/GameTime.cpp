//GameTime.cpp, the timer class

// ---- header files -----
#include "GameTime.h"
#include "Game/Globals.h"
// -----------------------

// extern ----------------
// -----------------------

const unsigned int kFixedTimeDelta = static_cast<int>( 1000.0f/60.0f );

GameTime& GameTime::Instance()
{
	static GameTime theTimeKeeper;
	return theTimeKeeper;
}

GameTime::~GameTime()
{
}

GameTime::GameTime()
	: timeDifference(0)
	, timeDifferenceAsPerCentOfSec(0.0f)
	, mTimeMode(eTMNormal)
	, elapsed(0.0f)
	, paused(false)
{
}

void GameTime::Reset()
{
	timeDifference = 0;
	timeDifferenceAsPerCentOfSec = 0.0f;
	elapsed = 0.0f;
}

float GameTime::GetElapsed()
{
	return elapsed;
}

void GameTime::Update()
{
	if (!paused)
	{
		UseFixedTime();
	}
	else
	{
		timeDifference = 0;
		timeDifferenceAsPerCentOfSec = 0.0f;
	}
	elapsed += timeDifferenceAsPerCentOfSec;

#ifdef DETERMINISTIC_REPLAY_DEBUG
	if (!paused)
	{
		ReplayManager::instance()->FloatResult(elapsed);
		ReplayManager::instance()->FloatResult(timeDifferenceAsPerCentOfSec);
	}
#endif
}

void GameTime::UseFixedTime()
{
	GetFixedTimeDelta(mTimeMode, timeDifference, timeDifferenceAsPerCentOfSec);
}

int GameTime::GetTimeDeltaMilli()
{
	return timeDifference;
}

float GameTime::GetTimeDeltaSeconds()
{
	return timeDifferenceAsPerCentOfSec;
}

int GameTime::GetCurrentFixedTimeDeltaMilli()
{
	unsigned int timeMilli = 0;
	float notUsed = 0.0f;
	GetFixedTimeDelta(mTimeMode, timeMilli, notUsed);
	return timeMilli;
}

float GameTime::GetCurrentFixedTimeDeltaSeconds()
{
	unsigned int notUsed = 0;
	float timeSeconds = 0.0f;
	GetFixedTimeDelta(mTimeMode, notUsed, timeSeconds);
	return timeSeconds;
}

int GameTime::GetFixedTimeDeltaMilli()
{
	unsigned int timeMilli = 0;
	float notUsed = 0.0f;
	GetFixedTimeDelta(eTMNormal, timeMilli, notUsed);
	return timeMilli;
}

float GameTime::GetFixedTimeDeltaSeconds()
{
	unsigned int notUsed = 0;
	float timeSeconds = 0.0f;
	GetFixedTimeDelta(eTMNormal, notUsed, timeSeconds);
	return timeSeconds;
}

void GameTime::GetFixedTimeDelta(eTimeMode mode, unsigned int& milliSeconds, float& seconds)
{
	switch(mode)
	{
	case eTMNormal:
		milliSeconds = kFixedTimeDelta;
		break;
	case eTMHalf:
		milliSeconds = kFixedTimeDelta/2;
		break;
	case eTMQuarter:
		milliSeconds = kFixedTimeDelta/4;
		break;
	case eTMEighth:
		milliSeconds = kFixedTimeDelta/8;
		break;
	case eTMSixteenth:
		milliSeconds = kFixedTimeDelta/16;
		break;
	default: break;
	}
	seconds = ConvertMilliSecondsToSeconds(milliSeconds);
}

float GameTime::ConvertMilliSecondsToSeconds(int milliSeconds)
{
	return ((float)milliSeconds*0.001f);
}

void GameTime::ProcessKeyInput()
{
#if _DEBUG
	if(Globals::Instance().keys.GetKeyPressed(eKeyCode_NUMPAD1))
	{
		mTimeMode = eTMNormal;
	}
	else if(Globals::Instance().keys.GetKeyPressed(eKeyCode_NUMPAD2))
	{
		mTimeMode = eTMHalf;
	}
	else if(Globals::Instance().keys.GetKeyPressed(eKeyCode_NUMPAD3))
	{
		mTimeMode = eTMQuarter;
	}
	else if(Globals::Instance().keys.GetKeyPressed(eKeyCode_NUMPAD4))
	{
		mTimeMode = eTMEighth;
	}
	else if(Globals::Instance().keys.GetKeyPressed(eKeyCode_NUMPAD5))
	{
		mTimeMode = eTMSixteenth;
	}
#endif
}

