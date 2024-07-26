#include "PlayerData.hpp"

using namespace Math;

bool PlayerData::loadFromFile( std::string filename )
{
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Couldn't open " << filename << std::endl;
        return false;

    } else {
        std::string line;
        std::getline(file, line);

        std::istringstream iss(line);
        iss >> std::dec >> mostNightsSurvived >> mostEnemiesKilled >> highscore;

        return true;
    }
}

bool PlayerData::Save( std::string filename )
{
    std::fstream file;
    file.open(filename, std::ios::out);

    if (!file) {
        std::cerr << "Failed to save to " << filename << std::endl;
        return false;
    
    } else {
        file << std::dec << mostNightsSurvived <<'\t'<< mostEnemiesKilled <<'\t'<< highscore;
        return true;
    }
}

void PlayerData::set_newHighscores(int nightsSurvived, int enemiesKilled, int score)
{
    mostNightsSurvived = Max(nightsSurvived, mostNightsSurvived);
    mostEnemiesKilled = Max(enemiesKilled, mostEnemiesKilled);
    highscore = Max(score, highscore);
}

void PlayerData::set_newHighscores(PlayerData scores)
{
    set_newHighscores(scores.mostNightsSurvived, scores.mostEnemiesKilled, scores.highscore);
}

void PlayerData::calculate_score()
{
    highscore = mostEnemiesKilled + (10 * mostNightsSurvived);
}

void PlayerData::reset()
{
    highscore = mostEnemiesKilled = mostNightsSurvived = 0;
}