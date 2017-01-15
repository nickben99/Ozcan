#ifndef _SOUNDCLASS_H_
#define _SOUNDCLASS_H_

#include <Audio/SoundsEnum.h>

struct ALLEGRO_SAMPLE;

class SoundClass
{
public:
	SoundClass();
	SoundClass(const SoundClass&);
	~SoundClass();

	bool Initialize();
    
    void Shutdown();
    
    bool LoadSound(const char* sound, SOUNDS id);

	bool PlaySound(SOUNDS eSoundToPlay_, bool bRepeat_, bool bForgroundSound_); // play a sound
	
private:
    ALLEGRO_SAMPLE *sample[SOUNDS_COUNT];
};
 
#endif