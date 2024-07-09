#include "Menu.hpp"

GameMenu::GameMenu( std::shared_ptr<LWindow> Window ) : window(Window)
{
    BGTexture = std::make_shared<LTexture>(window);
    if (!BGTexture->loadFromFile("../../assets/Bert.png")) {
        std::cerr << "Failed to load menu background!" << std::endl;
    }
}

void GameMenu::render_background()
{
    if (isActive) BGTexture->renderAsBackground();
}

void GameMenu::handle_events( SDL_Event &e, bool *menuActive )
{
    switch (e.type)
    {
        case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                *menuActive = isActive = !isActive;
            }
            break;
    }
}