// CKeyMapInterface.h

//system includes---
//------------------

#ifndef _CKeyMapInterface_h_
#define _CKeyMapInterface_h_

#ifdef _WINDOWS
#include "Input/PlatformSpecific/Windows/CKeyMapWindows.h"
#elif OSX
#include "Input/PlatformSpecific/OSX/CKeyMapOSX.h"
#else
#include "Input/PlatformSpecific/iOS/CKeyMapiOS.h"
#endif
#include "Input/CKeyCode.h"

class CKeyMapInterface
{ 
	public:
		bool Init()
		{
			return implementation.Init();
		}

		void Update()
		{
			implementation.Update();
		}

		void SetReplayManager(ReplayManager* replay)
		{
			implementation.SetReplayManager(replay);
		}

		bool GetKeyPressed(eKeyCode key, bool replaySynced = false)
		{ 
			return implementation.GetKeyPressed(key, replaySynced);
		}

		bool GetKeyReleased(eKeyCode key)
		{
			return implementation.GetKeyReleased(key);
		}

		bool GetKeyStatus(eKeyCode key, bool replaySynced = false)
		{
			return implementation.GetKeyStatus(key, replaySynced);
		}

private:
	CKeyMap implementation;
};

#endif // ifndef _CHighPrecisionTimerInterface_h_