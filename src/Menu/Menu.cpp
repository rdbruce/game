#include "Menu.hpp"
#include "Button.hpp"

GameMenu::GameMenu( std::shared_ptr<LWindow> Window, Game *game ) 
: window(Window), game(game)
{
    wRect = {0, 0, window->getWidth(), window->getHeight()};

    load_assets();

    load_data();

    // create buttons
    create_buttons();

    auto continueButton = menuButtons[0];
    if (mayContinue == continueButton->is_toggled()) continueButton->swap_textures();

    continueButton->apply_settings();
}

void GameMenu::create_buttons()
{
    confirmationText = "";
    if (currButtons == &confirmationButtons) {
        if (state == in_game) currButtons = &pauseButtons;
        else currButtons = &menuButtons;
    }

    pauseButtons.clear(); menuButtons.clear();
    gameOverButtons.clear(); settingsButtons.clear();
    confirmationButtons.clear();

    create_mainMenu_buttons();
    create_pauseMenu_buttons();
    create_settings_buttons();
}

void GameMenu::render_background()
{
    if (isActive) 
    {
        bool settingsOrMenu = state == main_menu || state == settings_menu;
        auto tex = (settingsOrMenu)? BGTexture :
                   tEditor.createSolidColour(wRect.w, wRect.h, 180, window);
        tex->render(wRect.x, wRect.y, &wRect);

        if (state == game_over) {
            SDL_Rect rect = {(wRect.w-GAMEOVER_TXT_WIDTH)/2 + wRect.x, 128, GAMEOVER_TXT_WIDTH, GAMEOVER_TEX_HEIGHT};
            gameOverTex->render(rect.x, rect.y, &rect);
        } 
        else if (settingsOrMenu && confirmationText == "") {
            SDL_Rect rect = {64 + wRect.x, 32 + wRect.y, 896, 192};
            titleTex->render(rect.x, rect.y, &rect);
        }
    }
}

void GameMenu::render_settings()
{
    if (state == settings_menu)
    {
        int x = 64 + wRect.x, y = 256;

        renderText("Audio", x, y, window, {255,255,255,255}, arcadeClassic36, Left_aligned);
        y += 40;

        auto white = tEditor.createSolidColour(464, 3, 0xFFFFFFFF, window);
        SDL_Rect rect = {x, y, 464, 3};
        white->render(x, y, &rect);
        y += 8;

        renderText("Volume", x, y, window, {255,255,255,255}, arcadeClassic24, Left_aligned);
        y += 90;

        auto grey = tEditor.createSolidColour(432, 3, 0xFFFFFFB0, window);
        rect.w = 432;
        grey->render(x + 16, y, &rect);

        y += 48;
        renderText("Music volume", x, y, window, {255,255,255,255}, arcadeClassic24, Left_aligned);
        y += 90;
        grey->render(x + 16, y, &rect);

        y += 96; rect.w = 464;
        renderText("Graphics", x, y, window, {255,255,255,255}, arcadeClassic36, Left_aligned);
        y += 40;

        white->render(x, y, &rect);
        y += 48;
        renderText("CRT Filter", x + CHECKBOX_SIDELENGTH + 16, y, window, {255,255,255,255}, arcadeClassic24, Left_aligned);

        y += CHECKBOX_SIDELENGTH + 16;
        renderText("Fullscreen", x + CHECKBOX_SIDELENGTH + 16, y, window, {255,255,255,255}, arcadeClassic24, Left_aligned);
        
        y += CHECKBOX_SIDELENGTH + 16;
        renderText("Show framerate", x + CHECKBOX_SIDELENGTH + 16, y, window, {255,255,255,255}, arcadeClassic24, Left_aligned);

        white->free();
        grey->free();
    }
}

void GameMenu::render_credits()
{
    if (state == main_menu && confirmationText == "")
    {
        std::string txt = "With music by wiredbeyondbelief!";
        int x = wRect.w/2 + wRect.x, y = wRect.h - 64;

        renderText(txt, x, y, window, {255,255,255,255}, arcadeClassic24);
    }
}

void GameMenu::render_highscores()
{
    if (state == main_menu && confirmationText == "") 
    {
        int x = 64 + wRect.x, y = 256;

        renderText("HIGHSCORES", HIGHSCORE_CENTREPOS+wRect.x, y, window, {255,0,0,255}, arcadeClassic48);
        y += 96; x += 148;

        renderText("SCORE", x, y, window, {255,0,0,255}, arcadeClassic24); x += 136;
        renderText("NIGHTS\nSURVIVED", x, y, window, {255,0,0,255}, arcadeClassic18); x += 128;
        renderText("ENEMIES\nKILLED", x, y, window, {255,0,0,255}, arcadeClassic18);

        x = 64 + wRect.x; y += 80;

        for (int i = 1; i <= num_highscores; i++)
        {
            if (set_score_name == i) 
            {
                highscores[i].render(x, y, window, {255,255,255,255}, arcadeClassic24, currChar, {255,0,0,255});
                x += 500;
                std::string str = "NEW HIGHSCORE!";
                renderText(str, x, y, window, {255,0,0,255}, arcadeClassic24, Left_aligned);
                x -= 500;
            } 
            else 
            {
                highscores[i].render(x, y, window, {255,255,255,255}, arcadeClassic24);
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
        
        renderText(txt, x, y, window, {255,255,255,255}, arcadeClassic24);
    }
}

void GameMenu::render_buttons()
{
    // find the coordinates of the mouse
    int x, y;
    get_mousePos(&x, &y);

    if (isActive && !set_score_name) 
    {
        int n = currButtons->size();
        for (int i = 0; i < n; i++) {
            //render the button
            auto button = (*currButtons)[i]; 
            button->render( wRect.x, wRect.y );

            // indicade which button (if any) is being hovered over
            if (button->isPressed(x, y)) 
            {
                int w = button->get_width(), h = button->get_height();
                if (w == BUTTON_WIDTH && h == BUTTON_HEIGHT && !button->is_toggled())
                {
                    std::string txt = ">";
                    auto tex = std::make_unique<LTexture>(window);
                    tex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic48);
                    int X, Y;
                    button->get_pos(&X, &Y);
                    
                    int rendx = X - tex->getWidth(), rendy = Y + (h-tex->getHeight())/2;
                    tex->render(rendx + wRect.x, rendy + wRect.y);

                    txt = "<";
                    tex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic48);
                    rendx = X + w;
                    tex->render(rendx + wRect.x, rendy + wRect.y);

                    tex->free();
                }
            }
        }
    }




}

void GameMenu::render_FPS()
{
    if (settings.flags&SHOW_FPS) game->render_framerate();
}

void GameMenu::render_CRT()
{
    if (settings.flags&CRT_FILTER) CRT_Tex->render(wRect.x, wRect.y, &wRect);
}

void GameMenu::enter_game_over()
{
    mayContinue = false;
    auto continueButton = menuButtons[0];
    if (mayContinue == continueButton->is_toggled()) continueButton->swap_textures();
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
                {
                    // WARNING!! this will cause the game to crash if you literally
                    // hold down F11 to spam fullscreen toggle, so don't do that!
                    fullscreen = window->toggleFullscreen();

                    sizeChange++;

                    if (state == in_game || state == game_over) {
                        game->initialise_BGTexture();
                    }
                    settings.flags &= ~FULLSCREEN;
                    settings.flags |= fullscreen << 4;

                    int idx = settingsButtons.size() - 4;
                    auto checkbox = settingsButtons[idx];
                    if (fullscreen != checkbox->is_toggled()) checkbox->swap_textures();

                    break;
                }

                default:
                    if (set_score_name != 0) rename_highscore(e.key.keysym.sym);
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if (e.button.button == SDL_BUTTON_LEFT) activeSlider = nullptr;
            break;
        
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT) leftClickFunc();
        
        default:
            if (activeSlider != nullptr && state == settings_menu) {
                activeSlider->execute_function();
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

        if (state == settings_menu)
        {
            int idx = settingsButtons.size()-1;
            if (settingsButtons[idx]->isPressed(x, y)) activeSlider = settingsButtons[idx];
            else if (settingsButtons[idx-1]->isPressed(x, y)) activeSlider = settingsButtons[idx-1];
        }

        // check all the buttons to see if they were clicked
        for (int i = 0; i < currButtons->size(); i++) {
            std::shared_ptr<Button> b = (*currButtons)[i];
            if (b->isPressed( x, y )) {
                // when pressed, execute the button's function
                b->execute_function();
                b->play_sound();
            }
        }
    }
}

bool GameMenu::is_active() { return isActive; }

void GameMenu::update()
{
    if (game->game_over() && state == in_game) enter_game_over();

    if (state == Quit) {
        save_data();
    }

    if (sizeChange) 
    {
        if (sizeChange == 2) 
        {
            create_buttons();

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

void GameMenu::load_data()
{
    // load highscores
    load_highscores();

    // load settings
    std::string filename = "../../saves/data/UserSettings.txt";
    std::ifstream file(filename);

    if (!file) std::cerr << "Couldn't open " << filename <<'\n';
    else 
    {
        settings.loadFromFile(&file);
        std::string line;
        std::getline(file, line);
        std::istringstream iss(line);
        iss >> mayContinue;
        file.close();
    }

    // commit loaded settings
    Mix_Volume(-1, settings.volume);

    if (settings.flags & FULLSCREEN) {
        fullscreen = window->toggleFullscreen();
        sizeChange++;
    }

    game->set_max_framerate(settings.max_framrate);
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

void GameMenu::save_data()
{
    save_highscores();

    std::string filename = "../../saves/data/UserSettings.txt";
    std::fstream file;
    file.open(filename, std::ios::out);

    if (!file) {
        std::cerr << "Failed to save to " << filename <<'\n';
    } else {
        settings.Save(&file);
        file << mayContinue;
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
    auto texture = tEditor.createMenuButton("CONTINUE", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    auto greyContinue = tEditor.createMenuButton("CONTINUE", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48, {100,100,100,255});
    
    int x = wRect.w - (9 * BUTTON_WIDTH/8), y = 256;
    SDL_Rect rect = {x, y, BUTTON_WIDTH, BUTTON_HEIGHT};
    auto button = std::make_shared<Button>(this, rect, texture, &Button::continue_game, buttonSound, greyContinue);
    menuButtons.push_back(button);
    // button->swap_textures();

    auto newGameTexture = tEditor.createMenuButton("NEW GAME", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    rect.y += 175;
    button = std::make_shared<Button>(this, rect, newGameTexture, &Button::new_game_confirmation, buttonSound);
    menuButtons.push_back(button);

    texture = tEditor.createMenuButton("SETTINGS", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    rect.y += 175;
    button = std::make_shared<Button>(this, rect, texture, &Button::go_to_settings, buttonSound);
    menuButtons.push_back(button);

    texture = tEditor.createMenuButton("EXIT", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    rect.y += 175;
    button = std::make_shared<Button>(this, rect, texture, &Button::exit_to_desktop, buttonSound);
    menuButtons.push_back(button);


    rect.x = HIGHSCORE_CENTREPOS - (BUTTON_WIDTH/2);
    texture = tEditor.createMenuButton("RESET", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    button = std::make_shared<Button>(this, rect, texture, &Button::reset_highscores_confirmation, buttonSound);
    menuButtons.push_back(button);

    rect = {(wRect.w-BUTTON_WIDTH)/2, wRect.h/2, BUTTON_WIDTH, BUTTON_HEIGHT};
    button = std::make_shared<Button>(this, rect, newGameTexture, &Button::load_new_game, buttonSound);
    gameOverButtons.push_back(button);
}

void GameMenu::create_pauseMenu_buttons()
{
    SDL_Rect rect = {(wRect.w-BUTTON_WIDTH)/2, 128, BUTTON_WIDTH, BUTTON_HEIGHT};

    auto texture = tEditor.createMenuButton("RESUME", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);

    auto button = std::make_shared<Button>(this, rect, texture, &Button::close_pause_menu, buttonSound);
    pauseButtons.push_back(button);

    auto MenuTexture = tEditor.createMenuButton("MAIN MENU", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic36);
    rect.y += 175;
    button = std::make_shared<Button>(this, rect, MenuTexture, &Button::exit_to_menu_confirmation, buttonSound);
    pauseButtons.push_back(button);


    rect.y = 256;
    texture = tEditor.createMenuButton("YES", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    button = std::make_shared<Button>(this, rect, texture, &Button::doNothing, buttonSound);
    confirmationButtons.push_back(button);

    rect.y += 175;
    texture = tEditor.createMenuButton("NO", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    button = std::make_shared<Button>(this, rect, texture, &Button::doNothing, buttonSound);
    confirmationButtons.push_back(button);

    rect = {(wRect.w-BUTTON_WIDTH)/2, (wRect.h/2)+175, BUTTON_WIDTH, BUTTON_HEIGHT};
    button = std::make_shared<Button>(this, rect, MenuTexture, &Button::go_to_main_menu_from_gameover, buttonSound);
    gameOverButtons.push_back(button);
}

void GameMenu::create_settings_buttons()
{
    int x = wRect.w - (9 * BUTTON_WIDTH/8), y = 256;
    SDL_Rect rect = {x, y, BUTTON_WIDTH, BUTTON_HEIGHT};

    auto texture = tEditor.createMenuButton("REVERT", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    auto button = std::make_shared<Button>(this, rect, texture, &Button::revert_settings, buttonSound);
    settingsButtons.push_back(button);
    rect.y += 175;

    texture = tEditor.createMenuButton("RESET", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    button = std::make_shared<Button>(this, rect, texture, &Button::reset_settings, buttonSound);
    settingsButtons.push_back(button);
    rect.y += 175;

    texture = tEditor.createMenuButton("RETURN", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    button = std::make_shared<Button>(this, rect, texture, &Button::go_to_mainMenu, buttonSound);
    settingsButtons.push_back(button);



    rect = {64, 700, CHECKBOX_SIDELENGTH, CHECKBOX_SIDELENGTH};
    
    auto unselected = std::make_shared<LTexture>(window),
         selected   = std::make_shared<LTexture>(window);
    if (!unselected->loadFromFile("../../assets/Menu/Buttons/UnselectedCheckbox.png")) {
        std::cerr << "Failed to load unselected checkbox!" << std::endl;
    }
    if (!selected->loadFromFile("../../assets/Menu/Buttons/SelectedCheckbox.png")) {
        std::cerr << "Failed to load selected checkbox!" << std::endl;
    }
    button = std::make_shared<Button>(this, rect, unselected, &Button::toggle_CRT, buttonSound, selected);
    settingsButtons.push_back(button);
    if (settings.flags&CRT_FILTER) button->swap_textures();

    rect.y += CHECKBOX_SIDELENGTH + 16;
    button = std::make_shared<Button>(this, rect, unselected, &Button::toggle_fullscreen, buttonSound, selected);
    settingsButtons.push_back(button);
    if (settings.flags&FULLSCREEN) button->swap_textures();
    
    rect.y += CHECKBOX_SIDELENGTH + 16;
    button = std::make_shared<Button>(this, rect, unselected, &Button::toggle_FPS, buttonSound, selected);
    settingsButtons.push_back(button);
    if (settings.flags&SHOW_FPS) button->swap_textures();
    

    int minX = 72, maxX = 504;
    float t = (float)settings.volume / MIX_MAX_VOLUME;
    x = (minX * (1.0f - t)) + (maxX * t);

    rect = {x, 361, SLIDER_WIDTH, SLIDER_HEIGHT};
    texture = tEditor.createSliderTexture(SLIDER_WIDTH, SLIDER_HEIGHT, window, {255,255,255,255});
    button = std::make_shared<Button>(this, rect, texture, &Button::volume_slider);
    settingsButtons.push_back(button);

    t = (float)settings.musicVolume / MIX_MAX_VOLUME;
    rect.x = (minX * (1.0f - t)) + (maxX * t);

    rect.y = 532 - SLIDER_HEIGHT/2;
    button = std::make_shared<Button>(this, rect, texture, &Button::music_volume_slider);
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
    
    arcadeClassic48 = TTF_OpenFont("../../assets/Fonts/ARCADECLASSIC.TTF", 48);
    if (arcadeClassic48 == NULL) {
        std::cerr << "Failed to load arcade classic font!" << std::endl;
    }

    arcadeClassic24 = TTF_OpenFont("../../assets/Fonts/ARCADECLASSIC.TTF", 24);
    if (arcadeClassic24 == NULL) {
        std::cerr << "Failed to load arcade classic font!" << std::endl;
    }
    
    arcadeClassic36 = TTF_OpenFont("../../assets/Fonts/ARCADECLASSIC.TTF", 36);
    if (arcadeClassic36 == NULL) {
        std::cerr << "Failed to load arcade classic font!" << std::endl;
    }
    arcadeClassic18 = TTF_OpenFont("../../assets/Fonts/ARCADECLASSIC.TTF", 18);
    if (arcadeClassic18 == NULL) {
        std::cerr << "Failed to load arcade classic font!" << std::endl;
    }

    titleTex = std::make_shared<LTexture>(window);
    if (!titleTex->loadFromFile("../../assets/Menu/Title.png")) {
        std::cerr << "Failed to load title text!" << std::endl;
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