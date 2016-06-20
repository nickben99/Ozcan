#ifdef _WINDOWS

#ifndef _SOUNDCLASS_H_
#define _SOUNDCLASS_H_

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include <Audio/SoundsEnum.h>

class SoundClass
{
public:
	SoundClass();
	SoundClass(const SoundClass&);
	~SoundClass();

	bool Initialize();

	bool PlaySound( SOUNDS eSoundToPlay_, bool bRepeat_, bool bForgroundSound_ = true ); // play a sound
 
private:
	void Shutdown();

	bool InitializeDirectSound(HWND);
	void ShutdownDirectSound();
 
	static bool LoadWaveFile(char*, IDirectSoundBuffer8**, IDirectSound8*);
	void ShutdownWaveFile(IDirectSoundBuffer8**);
 
	bool LoadSounds( void );
 
private:
	IDirectSound8* m_DirectSound;
	IDirectSoundBuffer* m_primaryBuffer;

	IDirectSoundBuffer8* m_secondaryBuffer[ SOUNDS_COUNT ];
};
 
#endif

#endif // #ifdef _WINDOWS