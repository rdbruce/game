#pragma once

#include "../engine/GameMath.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

struct PlayerData
{
    bool loadFromFile( std::string filename );
    bool Save( std::string filename );

    void set_newHighscores(int nightsSurvived, int enemiesKilled, int score);
    void set_newHighscores(PlayerData scores);
    void calculate_score();
    void reset();

    int mostNightsSurvived = 0;
    int mostEnemiesKilled = 0;
    int highscore = 0;
};