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
    if (chunk != NULL) {
        Mix_FreeChunk(chunk);
        chunk = NULL;
    }
}

bool LAudio::loadFromFile(std::string path)
{
    // get rid of preexisting audio
    free();

    // load the audio from file
    Mix_Chunk *newChunk = Mix_LoadWAV(path.c_str());

    if (newChunk == NULL) 
    {
        printf("Unable to create audio from %s! SDL Error: %s\n", path.c_str(), Mix_GetError());
    }

    // return success
    chunk = newChunk;
    return chunk != NULL;
}

void LAudio::play()
{
    Mix_PlayChannel(-1, chunk, 0);
}