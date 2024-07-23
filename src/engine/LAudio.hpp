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

        // accessors
        double get_duration();
        double get_position();

    private:

        // the actual audio
        Mix_Music *music = NULL;

        // the total duration of the audio
        double duration = 0.0;

        // the amount of time into the audio
        double position = 0.0;

        // whether the audio is currently playing
        bool isPlaying = false;
};