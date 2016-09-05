#ifdef OSX

#include "SoundOSX.h"

SoundClass::SoundClass()
{
}

SoundClass::SoundClass(const SoundClass&)
{
}
 
 
SoundClass::~SoundClass()
{
}
 
 
bool SoundClass::Initialize()
{
	return true;
}

// play a sound
bool SoundClass::PlaySound( SOUNDS, bool, bool )
{
	return false;
}

#endif