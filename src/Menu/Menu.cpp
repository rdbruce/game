#include "Menu.hpp"
#include "Button.hpp"

GameMenu::GameMenu( std::shared_ptr<LWindow> Window, Game *game ) 
: window(Window), game(game)
{
    BGTexture = std::make_shared<LTexture>(window);
    if (!BGTexture->loadFromFile("../../assets/Menu/MainMenuBG.png")) {
        std::cerr << "Failed to load menu background!" << std::endl;
    }

    gameOverTex = std::make_shared<LTexture>(window);
    if (!gameOverTex->loadFromFile("../../assets/Menu/GameOver.png")) {
        std::cerr << "Failed to load game over texture!" << std::endl;
    }

    buttonSound = std::make_shared<LAudio>();
    if (!buttonSound->loadFromFile("../../assets/Audio/ThinkFastChucklenuts.wav")) {
        std::cerr << "Failed to load button sound" << std::endl;
    }

    // create buttons
    create_mainMenu_buttons();
    create_pauseMenu_buttons();
}

void GameMenu::render_background()
{
    if (isActive) 
    {
        auto tex = (state == main_menu)? BGTexture :
                   tEditor.createSolidColour(window->getWidth(), window->getHeight(), 180, window);
        tex->renderAsBackground();

        if (state == game_over) {
            int x = window->getWidth()/2;
            SDL_Rect rect = {x-150, 75, 300, 166};
            gameOverTex->render(rect.x, rect.y, &rect);
        }
    }
}

void GameMenu::render_confirmation() {
    if (confirmationText != "") 
    {
        std::string txt = "Are you sure?\n" + confirmationText;
        int x = window->getWidth()/2, y = 65;
        
        renderText(txt, x, y, window);
    }
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

void GameMenu::enter_game_over()
{
    isActive = true;
    currButtons = &gameOverButtons;
    state = game_over;
}

bool GameMenu::handle_events( SDL_Event &e, bool *menuActive )
{
    if (game->game_over() && state == in_game) enter_game_over();

    switch (e.type)
    {
        case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_ESCAPE && state == in_game) {
                if (currButtons == &pauseButtons) isActive = !isActive;
                confirmationText = "";
                currButtons = &pauseButtons;
                game->clear_input();
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if (e.button.button == SDL_BUTTON_LEFT) leftClickFunc();
            break;
    }
    *menuActive = isActive;

    return state == Quit;
}

void GameMenu::leftClickFunc()
{
    if (isActive) {
        // find the coordinates of the mouse click
        int x, y;
        SDL_GetMouseState(&x, &y);

        // check all the buttons to see if they were clicked
        for (int i = 0; i < currButtons->size(); i++) {
            std::shared_ptr<Button> b = (*currButtons)[i];
            if (b->isPressed( x, y )) {
                // when pressed, execute the button's function
                b->execute_function();
                buttonSound->play();
            }
        }
    }
}

bool GameMenu::is_active() { return isActive; }

void GameMenu::create_mainMenu_buttons()
{
    auto texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/ContinueButton.png")) {
        std::cerr << "Failed to load continue button texture!" << std::endl;
    }
    
    int x = window->getWidth() - 325, y = 175;
    SDL_Rect rect = {x, y, 250, 83};
    auto button = std::make_shared<Button>(this, rect, texture, &Button::continue_game);
    menuButtons.push_back(button);
    

    auto newGameTexture = std::make_shared<LTexture>(window);
    if (!newGameTexture->loadFromFile("../../assets/Menu/Buttons/NewGameButton.png")) {
        std::cerr << "Failed to load new game button texture!" << std::endl;
    }
    rect.y += 125;
    button = std::make_shared<Button>(this, rect, newGameTexture, &Button::new_game_confirmation);
    menuButtons.push_back(button);

    texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/SettingsButton.png")) {
        std::cerr << "Failed to load settings button texture!" << std::endl;
    }
    rect.y += 125;
    button = std::make_shared<Button>(this, rect, texture);
    menuButtons.push_back(button);

    texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/ExitButton.png")) {
        std::cerr << "Failed to load texture for exit button" << std::endl;
    }
    rect.y += 125;
    button = std::make_shared<Button>(this, rect, texture, &Button::exit_to_desktop);
    menuButtons.push_back(button);


    rect.x = (window->getWidth()/2) - 125; rect.y = 250;
    button = std::make_shared<Button>(this, rect, newGameTexture, &Button::load_new_game);
    gameOverButtons.push_back(button);
}

void GameMenu::create_pauseMenu_buttons()
{
    int x = (window->getWidth()/2) - 125, y = 125;
    SDL_Rect rect = {x, y, 250, 83};

    auto texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/ResumeButton.png")) {
        std::cerr << "Failed to load resume button texture!" << std::endl;
    }

    auto button = std::make_shared<Button>(this, rect, texture, &Button::close_pause_menu);
    pauseButtons.push_back(button);

    auto MenuTexture = std::make_shared<LTexture>(window);
    if (!MenuTexture->loadFromFile("../../assets/Menu/Buttons/MainMenuButton.png")) {
        std::cerr << "failed to load main menu button texture!" << std::endl;
    }
    rect.y += 125;
    button = std::make_shared<Button>(this, rect, MenuTexture, &Button::exit_to_menu_confirmation);
    pauseButtons.push_back(button);


    rect.y = 175;
    texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/YesButton.png")) {
        std::cerr << "Failed to load texture for yes button!" << std::endl;
    }
    button = std::make_shared<Button>(this, rect, texture);
    confirmationButtons.push_back(button);

    rect.y += 125;
    texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/NoButton.png")) {
        std::cerr << "Failed to load texture for no button!" << std::endl;
    }
    button = std::make_shared<Button>(this, rect, texture);
    confirmationButtons.push_back(button);


    rect.y = 375;
    button = std::make_shared<Button>(this, rect, MenuTexture, &Button::go_to_mainMenu);
    gameOverButtons.push_back(button);
}

bool GameMenu::is_inGame() { return state == in_game || state == game_over; }