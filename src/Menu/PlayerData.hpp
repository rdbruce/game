#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

struct PlayerData
{
    bool loadFromFile( std::string filename );
    bool Save( std::string filename );

    void setScores(int nightsSurvived, int enemiesKilled);

    int mostNightsSurvived = 0;
    int mostEnemiesKilled = 0;
    int highscore = 0;
};