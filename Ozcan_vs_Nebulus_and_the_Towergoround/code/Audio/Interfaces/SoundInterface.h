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
#include <System/Interfaces/SystemInterface.h>

class SoundInterface
{ 
	public:
		bool Initialize()
		{
            if (!implementation.Initialize()) {
                return false;
            }
            
            char buffer[256];
            SPRINTF(buffer, "%ssounds/backgroundMusic.wav", GetDirectoryPath());
            if (!implementation.LoadSound( buffer, SOUNDS_MAINMUSIC)) {
                return false;}
            
            SPRINTF(buffer, "%ssounds/MENUITEM.wav", GetDirectoryPath());
            if (!implementation.LoadSound( buffer, SOUNDS_MENUCHANGE ) ){
                return false;}
            
            SPRINTF(buffer, "%ssounds/MENUSEL.wav", GetDirectoryPath());
            if (!implementation.LoadSound( buffer, SOUNDS_MENUSELECT ) ){
                return false;}
            
            SPRINTF(buffer, "%ssounds/cannon.wav", GetDirectoryPath());
            if (!implementation.LoadSound( buffer, SOUNDS_ENEMYEXPLODE ) ){
                return false;}
            
            SPRINTF(buffer, "%ssounds/ammopu.wav", GetDirectoryPath());
            if (!implementation.LoadSound( buffer, SOUNDS_KNOCKEDOFFPLATFORM ) ){
                return false;}
            
            SPRINTF(buffer, "%ssounds/laugh.wav", GetDirectoryPath());
            if (!implementation.LoadSound( buffer, SOUNDS_LOSELIFE ) ){
                return false;}
            
            SPRINTF(buffer, "%ssounds/HWEAPON0.wav", GetDirectoryPath());
            if (!implementation.LoadSound( buffer, SOUNDS_THROWFIREBALL ) ){
                return false;}
            
            SPRINTF(buffer, "%ssounds/BEEP2.wav", GetDirectoryPath());
            if (!implementation.LoadSound( buffer, SOUNDS_COLLECTPOWERUP ) ){
                return false;}
            
            return true; // success
		}
    
        void Shutdown() {
            implementation.Shutdown();
        }

		bool PlaySound( SOUNDS eSoundToPlay_, bool bRepeat_, bool bForgroundSound_ = true ) // play a sound
		{
			return implementation.PlaySound(eSoundToPlay_, bRepeat_, bForgroundSound_); // play a sound
		}

private:
	SoundClass implementation;
};

#endif // ifndef _SoundInterface_h_