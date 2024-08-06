#include "Menu.hpp"
#include "Button.hpp"

GameMenu::GameMenu( std::shared_ptr<LWindow> Window, Game *game ) 
: window(Window), game(game)
{
    wRect = {0, 0, window->getWidth(), window->getHeight()};

    load_assets();

    // create buttons
    create_mainMenu_buttons();
    create_pauseMenu_buttons();
    create_settings_buttons();

    // load player highscores
    load_highscores();
}

void GameMenu::render_background()
{
    if (isActive) 
    {
        auto tex = (state == main_menu || state == settings)? BGTexture :
                   tEditor.createSolidColour(wRect.w, wRect.h, 180, window);
        tex->render(wRect.x, wRect.y, &wRect);

        if (state == game_over) {
            SDL_Rect rect = {(wRect.w-GAMEOVER_TXT_WIDTH)/2 + wRect.x, 128, GAMEOVER_TXT_WIDTH, GAMEOVER_TEX_HEIGHT};
            gameOverTex->render(rect.x, rect.y, &rect);
        }
    }
}

void GameMenu::render_settings()
{
    if (state == settings)
    {
        int x = 64 + wRect.x, y = 256;

        renderText("Audio", x, y, window, {255,255,255,255}, NULL, Left_aligned);
        y += 35;

        auto white = tEditor.createSolidColour(464, 3, 0xFFFFFFFF, window);
        SDL_Rect rect = {x, y, 464, 3};
        white->render(x, y, &rect);
        y += 8;

        renderText("Volume", x, y, window, {255,255,255,255}, NULL, Left_aligned);
        y += 90;

        auto grey = tEditor.createSolidColour(432, 3, 0x0F0F0FF0, window);
        rect.w = 432;
        grey->render(x + 16, y, &rect);

        white->free();
        grey->free();
    }
}

void GameMenu::render_highscores()
{
    if (state == main_menu && confirmationText == "") 
    {
        int x = 64 + wRect.x, y = 256;

        renderText("HIGHSCORES", HIGHSCORE_CENTREPOS+wRect.x, y, window, {255,0,0,255}, sevenSegment48);
        y += 55; x += 128;

        renderText("SCORE", x, y, window, {255,0,0,255}, sevenSegment36); x += 128;
        renderText("NIGHTS\nSURVIVED", x, y, window, {255,0,0,255}, sevenSegment24); x += 128;
        renderText("ENEMIES\nKILLED", x, y, window, {255,0,0,255}, sevenSegment24);

        x = 64 + wRect.x; y += 96;

        for (int i = 1; i <= num_highscores; i++)
        {
            if (set_score_name == i) 
            {
                highscores[i].render(x, y, window, {255,255,255,255}, sevenSegment36, currChar, {255,0,0,255});
                x += 425;
                std::string str = "NEW HIGHSCORE!";
                renderText(str, x, y, window, {255,0,0,255}, sevenSegment36, Left_aligned);
                x -= 425;
            } 
            else 
            {
                highscores[i].render(x, y, window, {255,255,255,255}, sevenSegment36);
            }
            y += 64;
        }
    }
}

void GameMenu::render_confirmation() {
    if (confirmationText != "") 
    {
        std::string txt = "Are you sure?\n" + confirmationText;
        int x = (wRect.w/2) + wRect.x, y = 65 + wRect.y;
        
        renderText(txt, x, y, window);
    }
}

void GameMenu::render_buttons()
{
    if (isActive && !set_score_name) 
    {
        int n = currButtons->size();
        for (int i = 0; i < n; i++) {
            (*currButtons)[i]->render( wRect.x, wRect.y );
        }
    }
}

void GameMenu::render_CRT()
{
    CRT_Tex->render(wRect.x, wRect.y, &wRect);
}

void GameMenu::enter_game_over()
{
    isActive = true;
    currButtons = &gameOverButtons;
    state = game_over;
    confirmationText = "";
}

void GameMenu::render_aspect_ratio()
{
    int w = wRect.x, h = wRect.h;
    if (w) 
    {
        SDL_Rect rect = {0, 0, w, h};
        
        aspectRatio->render(rect.x, rect.y, &rect);
        rect.x += rect.w + wRect.w;
        aspectRatio->render(rect.x, rect.y, &rect);
    }
}

bool GameMenu::handle_events( SDL_Event &e, bool *menuActive )
{
    switch (e.type)
    {
        case SDL_KEYDOWN:

            switch (e.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    if (state == in_game) {
                        if (currButtons == &pauseButtons) isActive = !isActive;
                        confirmationText = "";
                        currButtons = &pauseButtons;
                        game->clear_input();
                    }
                    break;

                case SDLK_F11:
                    // WARNING!! this will cause the game to crash if you literally
                    // hold down F11 to spam fullscreen toggle, so don't do that!
                    fullscreen = window->toggleFullscreen();

                    sizeChange++;

                    if (state == in_game || state == game_over) {
                        game->initialise_BGTexture();
                    }
                    break;

                default:
                    if (set_score_name != 0) rename_highscore(e.key.keysym.sym);
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if (e.button.button == SDL_BUTTON_LEFT) isOnVolumeSlider = false;
            break;
        
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT) leftClickFunc();
        
        default:
            if (isOnVolumeSlider && currButtons == &settingsButtons) {
                int idx = settingsButtons.size()-1;
                settingsButtons[idx]->execute_function();
            }
            break;
    }

    *menuActive = isActive;

    return state == Quit;
}

void GameMenu::rename_highscore( SDL_Keycode sym )
{
    char *ch = &highscores[set_score_name].name[currChar];
    switch (sym)
    {
        case SDLK_UP:
            if (*ch == '_' || *ch == 'Z') *ch = 'A';
            else (*ch)++;
            break;

        case SDLK_DOWN:
            if (*ch == '_' || *ch == 'A') *ch = 'Z';
            else (*ch)--;
            break;

        case SDLK_BACKSPACE:
            *ch = '_';
        case SDLK_LEFT:
            currChar = Max(0, currChar-1);
            break;

        case SDLK_RIGHT:
            currChar = Min(2, currChar+1);
            break;

        case SDLK_RETURN:
            if (currChar < 2) currChar++;
            else {
                int i;
                std::string name = highscores[set_score_name].name;
                for (i = 0; i < 3; i++) {
                    if (name[i] == '_') break;
                }
                if (i == 3) set_score_name = currChar = 0;
            }
            break;

        default:
            if (sym >= SDLK_a && sym <= SDLK_z) {
                *ch = (int)sym - 32; 
                currChar = Min(2, currChar+1);
            }
    }
}

void GameMenu::get_mousePos( int *x, int *y)
{
    SDL_GetMouseState(x, y);
    *x /= window->getScaleX(); *y /= window->getScaleY();
    *x -= wRect.x; *y -= wRect.y;
}

void GameMenu::leftClickFunc()
{
    if (isActive && !set_score_name) 
    {
        // find the coordinates of the mouse click
        int x, y;
        get_mousePos(&x, &y);

        if (currButtons == &settingsButtons)
        {
            int idx = settingsButtons.size()-1;
            if (settingsButtons[idx]->isPressed(x, y)) isOnVolumeSlider = true;
        }

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

void GameMenu::update()
{
    if (game->game_over() && state == in_game) enter_game_over();

    if (state == Quit) {
        save_highscores();
    }

    if (sizeChange) 
    {
        if (sizeChange == 2) 
        {
            create_CRT_Texture();

            if (aspectRatio != nullptr) aspectRatio->free();

            if (fullscreen) 
            {
                int w = window->getWidth(), h = window->getHeight(),
                    s = wRect.w * window->getScaleX();

                    wRect.x = (w - s)/2; wRect.y = 0;

                    aspectRatio = tEditor.createSolidColour(w, h, 0x000000FF, window);
            }
            else
            {
                wRect.x = wRect.y = 0;
            }

            game->renderOffset = Vector2Int(wRect.x, wRect.y);
            sizeChange = 0;
        } else sizeChange++;
    }
}

void GameMenu::load_highscores()
{
    std::string filename = "../../saves/data/High_Scores.txt";
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Couldn't open " << filename << std::endl;

    } else {
        std::string line;
        std::getline(file, line);

        std::istringstream iss(line);

        PlayerData currGame;
        currGame.loadFromIss( &iss );
        highscores[0] = currGame;

        std::getline(file, line);
        iss = std::istringstream(line);
        iss >> num_highscores;

        for (int i = 1; i <= num_highscores; i++)
        {
            std::getline(file, line);
            iss = std::istringstream(line);
            PlayerData data;
            data.loadFromIss(&iss);
            highscores[i] = data;
        }
    }
}

void GameMenu::save_highscores()
{
    std::fstream file;
    std::string filename = "../../saves/data/High_Scores.txt";
    file.open(filename, std::ios::out);

    if (!file) 
    {
        std::cerr << "Failed to save to " << filename << std::endl;
    } 
    else 
    {
        highscores[0].Save(&file);

        file <<std::dec<< num_highscores << '\n';

        for (int i = 1; i <= num_highscores; i++) {
            highscores[i].Save(&file);
        }

        file.close();
    }
}

int GameMenu::new_highscore()
{
    int newScore = highscores[0].highscore,
        newEnemyCount = highscores[0].mostEnemiesKilled,
        newNightCount = highscores[0].mostNightsSurvived;

    int i;
    for (i = num_highscores; i > 0; i--) 
    {
        PlayerData curr = highscores[i];

        // this used to be a huge if-else chain but i did some proofs with
        // propositional logic and made it far more elegant lol

        bool p = newScore < curr.highscore,
             q = newScore == curr.highscore,
             a = newNightCount < curr.mostNightsSurvived,
             b = newNightCount == curr.mostNightsSurvived,
             c = newEnemyCount <= curr.mostEnemiesKilled;

        bool eval = p || (q && (a || ( b && c)));

        if (eval) break;
    }
    return (i + 1)%6;
}

void GameMenu::create_mainMenu_buttons()
{
    auto texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/ContinueButton.png")) {
        std::cerr << "Failed to load continue button texture!" << std::endl;
    }
    
    int x = wRect.w - (9 * BUTTON_WIDTH/8), y = 256;
    SDL_Rect rect = {x, y, BUTTON_WIDTH, BUTTON_HEIGHT};
    auto button = std::make_shared<Button>(this, rect, texture, &Button::continue_game);
    menuButtons.push_back(button);
    

    auto newGameTexture = std::make_shared<LTexture>(window);
    if (!newGameTexture->loadFromFile("../../assets/Menu/Buttons/NewGameButton.png")) {
        std::cerr << "Failed to load new game button texture!" << std::endl;
    }
    rect.y += 175;
    button = std::make_shared<Button>(this, rect, newGameTexture, &Button::new_game_confirmation);
    menuButtons.push_back(button);

    texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/SettingsButton.png")) {
        std::cerr << "Failed to load settings button texture!" << std::endl;
    }
    rect.y += 175;
    button = std::make_shared<Button>(this, rect, texture, &Button::go_to_settings);
    menuButtons.push_back(button);

    texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/ExitButton.png")) {
        std::cerr << "Failed to load texture for exit button" << std::endl;
    }
    rect.y += 175;
    button = std::make_shared<Button>(this, rect, texture, &Button::exit_to_desktop);
    menuButtons.push_back(button);


    rect.x = HIGHSCORE_CENTREPOS - (BUTTON_WIDTH/2);
    texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/ResetButton.png")) {
        std::cerr << "failed to load reset button texture!" << std::endl;
    }
    button = std::make_shared<Button>(this, rect, texture, &Button::reset_highscores_confirmation);
    menuButtons.push_back(button);

    rect = {(wRect.w-BUTTON_WIDTH)/2, wRect.h/2, BUTTON_WIDTH, BUTTON_HEIGHT};
    button = std::make_shared<Button>(this, rect, newGameTexture, &Button::load_new_game);
    gameOverButtons.push_back(button);
}

void GameMenu::create_pauseMenu_buttons()
{
    SDL_Rect rect = {(wRect.w-BUTTON_WIDTH)/2, 128, BUTTON_WIDTH, BUTTON_HEIGHT};

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
    rect.y += 175;
    button = std::make_shared<Button>(this, rect, MenuTexture, &Button::exit_to_menu_confirmation);
    pauseButtons.push_back(button);


    rect.y = 256;
    texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/YesButton.png")) {
        std::cerr << "Failed to load texture for yes button!" << std::endl;
    }
    button = std::make_shared<Button>(this, rect, texture);
    confirmationButtons.push_back(button);

    rect.y += 175;
    texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/NoButton.png")) {
        std::cerr << "Failed to load texture for no button!" << std::endl;
    }
    button = std::make_shared<Button>(this, rect, texture);
    confirmationButtons.push_back(button);

    rect = {(wRect.w-BUTTON_WIDTH)/2, (wRect.h/2)+175, BUTTON_WIDTH, BUTTON_HEIGHT};
    button = std::make_shared<Button>(this, rect, MenuTexture, &Button::go_to_main_menu_from_gameover);
    gameOverButtons.push_back(button);
}

void GameMenu::create_settings_buttons()
{
    int x = wRect.w - (9 * BUTTON_WIDTH/8), y = 256;
    SDL_Rect rect = {x, y, BUTTON_WIDTH, BUTTON_HEIGHT};

    auto texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/ControlsButton.png")) {
        std::cerr << "Failed to load controls button" <<'\n';
    }
    auto button = std::make_shared<Button>(this, rect, texture);
    settingsButtons.push_back(button);
    rect.y += 175;

    texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/RevertButton.png")) {
        std::cerr << "Failed to load revert settings button" <<'\n';
    }
    button = std::make_shared<Button>(this, rect, texture);
    settingsButtons.push_back(button);
    rect.y += 175;

    texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/ResetButton.png")) {
        std::cerr << "Failed to load reset settings button" <<'\n';
    }
    button = std::make_shared<Button>(this, rect, texture);
    settingsButtons.push_back(button);
    rect.y += 175;

    texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/ReturnButton.png")) {
        std::cerr << "Failed to load return button" <<'\n';
    }
    button = std::make_shared<Button>(this, rect, texture, &Button::go_to_mainMenu);
    settingsButtons.push_back(button);

    rect = {504, 356, SLIDER_WIDTH, SLIDER_HEIGHT};
    texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/Slider.png")) {
        std::cerr << "Failed to load volume slider" <<'\n';
    }
    button = std::make_shared<Button>(this, rect, texture, &Button::volume_slider);
    settingsButtons.push_back(button);

}

void GameMenu::load_assets()
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

    sevenSegment48 = TTF_OpenFont("../../assets/Fonts/Seven_Segment.ttf", 48);
    if (sevenSegment48 == NULL) {
        std::cerr << "Failed to load seven segment font!" << std::endl;
    }

    sevenSegment24 = TTF_OpenFont("../../assets/Fonts/Seven_Segment.ttf", 24);
    if (sevenSegment24 == NULL) {
        std::cerr << "Failed to load seven segment font!" << std::endl;
    }
    
    sevenSegment36 = TTF_OpenFont("../../assets/Fonts/Seven_Segment.ttf", 36);
    if (sevenSegment36 == NULL) {
        std::cerr << "Failed to load seven segment font!" << std::endl;
    }


    CRT_Base = std::make_shared<LTexture>(window);
    if (!CRT_Base->loadFromFile("../../assets/CRT_Base_Texture.png")) {
        std::cerr << "Failed to load CRT Pixel texture!" << std::endl;
    }

    create_CRT_Texture();

    int w = wRect.x, h = wRect.h;
    aspectRatio = tEditor.createSolidColour(w, h, 0x000000FF, window);
}

void GameMenu::create_CRT_Texture()
{
    CRT_Tex = tEditor.createEmptyTexture(wRect.w, wRect.h, window);
    int nx = CRT_Tex->getWidth() / CRT_Base->getWidth(), ny =( CRT_Tex->getHeight() / CRT_Base->getWidth())+2;

    for (int i = 0; i <= nx; i++) {
        int x = i * CRT_Base->getWidth();
        for (int j = 0; j <= ny; j++) {
            int y = j * CRT_Base->getHeight();
            SDL_Rect rect = { x, y, CRT_Base->getWidth(), CRT_Base->getHeight() };
            tEditor.renderTextureToTexture(CRT_Tex, CRT_Base, &rect);
        }
    }
    CRT_Tex->setAlpha(20);
}

bool GameMenu::is_inGame() { return state == in_game || state == game_over; }