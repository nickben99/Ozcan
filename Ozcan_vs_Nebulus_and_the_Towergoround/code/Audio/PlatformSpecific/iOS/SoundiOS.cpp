#include "SoundiOS.h"
#include "Game/ReplayManager.h"
#include "Game/defines.h"

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