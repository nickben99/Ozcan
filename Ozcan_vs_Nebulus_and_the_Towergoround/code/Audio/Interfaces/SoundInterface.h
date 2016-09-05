// SoundInterface.h

//system includes---
//------------------

#ifndef _SoundInterface_h_
#define _SoundInterface_h_

#ifdef _WINDOWS
#include "Audio/PlatformSpecific/Windows/SoundWindows.h"
#elif OSX
#include "Audio/PlatformSpecific/OSX/SoundOSX.h"
#else
#include "Audio/PlatformSpecific/iOS/SoundiOS.h"
#endif
#include "Audio/SoundsEnum.h"

class SoundInterface
{ 
	public:
		bool Initialize()
		{
			return implementation.Initialize();
		}

		bool PlaySound( SOUNDS eSoundToPlay_, bool bRepeat_, bool bForgroundSound_ = true ) // play a sound
		{
			return implementation.PlaySound(eSoundToPlay_, bRepeat_, bForgroundSound_); // play a sound
		}

private:
	SoundClass implementation;
};

#endif // ifndef _SoundInterface_h_