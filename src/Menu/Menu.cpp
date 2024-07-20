#include "Menu.hpp"
#include "Button.hpp"

GameMenu::GameMenu( std::shared_ptr<LWindow> Window, Game *game ) 
: window(Window), game(game)
{
    BGTexture = std::make_shared<LTexture>(window);
    if (!BGTexture->loadFromFile("../../assets/Bert.png")) {
        std::cerr << "Failed to load menu background!" << std::endl;
    }

    // create buttons
    auto texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/ContinueButton.png")) {
        std::cerr << "Failed to load continue button texture!" << std::endl;
    }
    
    int x = window->getWidth() - 275, y = 75;
    SDL_Rect rect = {x, y, 200, 66};
    auto button = std::make_shared<Button>(this, rect, texture, &Button::continue_game);
    menuButtons.push_back(button);
    

    texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/NewGameButton.png")) {
        std::cerr << "Failed to load new game button texture!" << std::endl;
    }
    rect.y += 99;
    button = std::make_shared<Button>(this, rect, texture, &Button::load_new_game);
    menuButtons.push_back(button);
}

void GameMenu::render_background()
{
    if (isActive) BGTexture->renderAsBackground();
}

void GameMenu::render_buttons()
{
    if (isActive) {
        int n = currButtons->size();
        for (int i = 0; i < n; i++) {
            (*currButtons)[i]->render();
        }
    }
}

void GameMenu::handle_events( SDL_Event &e, bool *menuActive )
{
    switch (e.type)
    {
        case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_ESCAPE && state == in_game) {
                isActive = !isActive;
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if (e.button.button == SDL_BUTTON_LEFT) leftClickFunc();
            break;
    }
    *menuActive = isActive;
}

void GameMenu::leftClickFunc()
{
    // find the coordinates of the mouse click
    int x, y;
    SDL_GetMouseState(&x, &y);

    // check all the buttons to see if they were clicked
    int n = currButtons->size();
    for (int i = 0; i < n && isActive; i++) {
        std::shared_ptr<Button> b = (*currButtons)[i];
        if (b->isPressed( x, y )) {
            // when pressed, execute the button's function
            b->execute_function();
        }
    }
}

bool GameMenu::is_active() { return isActive; }