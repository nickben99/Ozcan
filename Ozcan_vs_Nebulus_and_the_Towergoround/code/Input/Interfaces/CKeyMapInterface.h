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
		CKeyMapInterface() 
			: mpReplay(nullptr)
		{

		}

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
			mpReplay = replay;
		}

		bool GetKeyPressed(eKeyCode key, bool replaySynced = false)
		{
			if (replaySynced && nullptr != mpReplay)
			{
				if (mpReplay->IsRecording())
				{
					mpReplay->AddPressedQuery(key, implementation.GetKeyPressed(key));
				}
				return mpReplay->GetPressedQuery(key);
			}
			return implementation.GetKeyPressed(key);
		}

		bool GetKeyReleased(eKeyCode key)
		{
			return implementation.GetKeyReleased(key);
		}

		bool GetKeyStatus(eKeyCode key, bool replaySynced = false)
		{
			if (replaySynced && nullptr != mpReplay)
			{
				if (mpReplay->IsRecording())
				{
					mpReplay->AddStatusQuery(key, implementation.GetKeyStatus(key));
				}
				return mpReplay->GetStatusQuery(key);
			}
			return implementation.GetKeyStatus(key);
		}

private:
	CKeyMap implementation;
	ReplayManager* mpReplay;
};

#endif // ifndef CKeyMapInterface