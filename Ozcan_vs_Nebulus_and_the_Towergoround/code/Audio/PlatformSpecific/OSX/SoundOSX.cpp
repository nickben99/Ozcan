#include "SoundOSX.h"

#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

SoundClass::SoundClass()
{
    for (int index = 0; index < SOUNDS_COUNT; ++index)
    {
        sample[ index ] = nullptr;
    }
}

SoundClass::SoundClass(const SoundClass&)
{
}
 
 
SoundClass::~SoundClass()
{
}

void SoundClass::Shutdown()
{
    for (int index = 0; index < SOUNDS_COUNT; ++index)
    {
        if (sample[index]) {
            al_destroy_sample(sample[ index ]);
        }
    }
    
    al_uninstall_audio();
}
 
bool SoundClass::Initialize()
{
    if(!al_install_audio()){
        return false;
    }

    if(!al_init_acodec_addon()){
        return false;
    }
    
    if (!al_reserve_samples(SOUNDS_COUNT)){
        return false;
    }
	return true;
}

bool SoundClass::LoadSound(const char* soundFile, SOUNDS id) {
    sample[id] = al_load_sample(soundFile);
    return sample[id];
}

// play a sound
bool SoundClass::PlaySound(SOUNDS eSoundToPlay_, bool bRepeat_, bool bForgroundSound_)
{
    float volume = bForgroundSound_ ? 1.0f : 0.6f;
    const float playSpeed = 1.0f;
    
    if (bRepeat_) {
        al_play_sample(sample[eSoundToPlay_], volume, ALLEGRO_AUDIO_PAN_NONE, playSpeed, ALLEGRO_PLAYMODE_LOOP, NULL);
    } else {
        al_play_sample(sample[eSoundToPlay_], volume, ALLEGRO_AUDIO_PAN_NONE, playSpeed, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
    return true;
}