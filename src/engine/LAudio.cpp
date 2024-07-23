#include "LAudio.hpp"
#include <iostream>

LAudio::LAudio()
{
    // open that audio device for playback
    Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
}

LAudio::~LAudio()
{
    free();
}

void LAudio::free()
{
    if (music != NULL) {
        Mix_FreeMusic(music);
        isPlaying = false;
        music = NULL;
        position = duration = 0.0;
    }
}

bool LAudio::loadFromFile(std::string path)
{
    // get rid of preexisting audio
    free();

    // load the audio from file
    Mix_Music *newMusic = Mix_LoadMUS(path.c_str());

    if (newMusic == NULL) 
    {
        printf("Unable to create audio from %s! SDL Error: \n", path.c_str(), Mix_GetError());
    }
    else 
    {
        duration = Mix_MusicDuration(newMusic);
    }

    // return success
    music = newMusic;
    return music != NULL;
}

void LAudio::play()
{
    if (!isPlaying) {
        isPlaying = true;
        Mix_PlayMusic(music, 1);
    }
    if (position == duration) isPlaying = false;
    position = Mix_GetMusicPosition(music);
}

double LAudio::get_duration() { return duration; }
double LAudio::get_position() { return position; }