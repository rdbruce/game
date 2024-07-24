#pragma once

#include <SDL.h>
#include <SDL_mixer.h>

#include <string>

class LAudio
{
    public:
        // initialises variables
        LAudio();

        // deallocates memory
        ~LAudio();

        // loads audio from an mp3 file
        bool loadFromFile(std::string path);

        // deallocates audio
        void free();

        // plays the audio
        void play();

    private:

        // the actual audio
        Mix_Chunk *chunk = NULL;
};