#ifndef _SOUNDCLASS_H_
#define _SOUNDCLASS_H_

#include <Audio/SoundsEnum.h>

class SoundClass
{
public:
	SoundClass();
	SoundClass(const SoundClass&);
	~SoundClass();

	bool Initialize();

	bool PlaySound( SOUNDS eSoundToPlay_, bool bRepeat_, bool bForgroundSound_ = true ); // play a sound
	
};
 
#endif