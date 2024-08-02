#pragma once

#include "../engine/GameMath.hpp"
#include "../engine/LTexture.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

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