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
        iss >> name >>std::dec>> mostNightsSurvived >> mostEnemiesKilled >> highscore;

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
    name = "---";
    highscore = mostEnemiesKilled = mostNightsSurvived = 0;
}

void PlayerData::loadFromIss( std::istringstream *iss )
{
    *iss >> name >>std::dec>> mostNightsSurvived >> mostEnemiesKilled >> highscore;
}

void PlayerData::Save( std::fstream *file )
{
    *file << name <<'\t'<<std::dec<< mostNightsSurvived <<'\t'<< mostEnemiesKilled <<'\t'<< highscore <<'\n';
}

void PlayerData::render(int x, int y, std::shared_ptr<LWindow> window, SDL_Color colour, TTF_Font *font, int colouredLetter, SDL_Color altColor)
{
    if (colouredLetter != -1) 
    {
        int n = name.size(), X = x;
        for (int i = 0; i < n; i++) 
        {
            std::string str = {name[i]};
            SDL_Color col = (i == colouredLetter)? altColor : colour;
            auto rend = std::make_unique<LTexture>(window);
            if (!rend->loadFromRenderedText(str, col, font)) {
                std::cerr << "failed to load rendered text!" << std::endl;
            }
            rend->render(X, y); 
            X += rend->getWidth();
        }
    } 
    else {
        renderText(name, x, y, window, colour, font, Left_aligned);
    }
    x += 150;

    std::string txt;
    if (name == "---") 
    {
        txt = "-";
        renderText(txt, x, y, window, colour, font, Left_aligned); x += 75;
        renderText(txt, x, y, window, colour, font, Left_aligned); x += 75;
        renderText(txt, x, y, window, colour, font, Left_aligned);
    }
    else 
    {
        std::string txt = std::to_string(highscore);
        renderText(txt, x, y, window, colour, font, Left_aligned); x += 75;
        txt = std::to_string(mostNightsSurvived);
        renderText(txt, x, y, window, colour, font, Left_aligned); x += 75;
        txt = std::to_string(mostEnemiesKilled);
        renderText(txt, x, y, window, colour, font, Left_aligned);
    }
}