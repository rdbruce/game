#include "PlayerData.hpp"

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
        iss >> std::dec >> mostNightsSurvived >> mostEnemiesKilled;

        highscore = mostEnemiesKilled + (10 * mostNightsSurvived);

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
        file << std::dec << mostNightsSurvived <<'\t'<< mostEnemiesKilled;
        return true;
    }
}

void PlayerData::setScores(int nightsSurvived, int enemiesKilled)
{
    mostNightsSurvived = nightsSurvived;
    mostEnemiesKilled = enemiesKilled;
    highscore = enemiesKilled + (10 * mostNightsSurvived);
}