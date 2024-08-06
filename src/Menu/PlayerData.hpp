#pragma once

#include "../engine/GameMath.hpp"
#include "../engine/LTexture.hpp"

#include <SDL_mixer.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#define FULLSCREEN              0b00010000
#define PLAY_MUSIC              0b00001000
#define MUTE_WHILE_UNFOCUSED    0b00000100
#define CRT_FILTER              0b00000010
#define SHOW_FPS                0b00000001

struct PlayerData
{
    bool loadFromFile( std::string filename );
    void loadFromIss( std::istringstream *iss );

    void Save( std::fstream *file );

    void set_newHighscores(int nightsSurvived, int enemiesKilled, int score);
    void set_newHighscores(PlayerData scores);
    void calculate_score();
    void reset();

    void render(int x, int y, std::shared_ptr<LWindow> window, SDL_Color colour = {255,255,255,255}, TTF_Font *font = NULL, int colouredLetter = -1, SDL_Color altColor = {255,255,255,255});

    std::string name = "---";

    int mostNightsSurvived = 0;
    int mostEnemiesKilled = 0;
    int highscore = 0;
};


struct Settings
{
    bool loadFromFile( std::string filename );
    void Save( std::string filename );

    // sets all members to default settings
    void reset();

    int volume = MIX_MAX_VOLUME;
    /* 0 - 0 - 0 - 0 - music - mute while unfocused - CRT filter - display FPS */
    int flags = 10;

    int max_framrate = -1;
};