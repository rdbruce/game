#include "Menu.hpp"
#include "Button.hpp"

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
    Mix_Volume(0, ((float)settings.volume/MIX_MAX_VOLUME)*settings.musicVolume);

    if (settings.flags & FULLSCREEN) {
        fullscreen = window->toggleFullscreen();
        sizeChange++;
    }

    game->set_max_framerate(settings.max_framrate);
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

void GameMenu::create_mainMenu_buttons()
{
    auto texture = tEditor.createMenuButton("CONTINUE", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    auto greyContinue = tEditor.createMenuButton("CONTINUE", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48, {100,100,100,255});
    
    int x = wRect.w - (9 * BUTTON_WIDTH/8), y = 256;
    SDL_Rect rect = {x, y, BUTTON_WIDTH, BUTTON_HEIGHT};
    auto button = std::make_shared<Button>(this, rect, greyContinue, &Button::continue_game, arcadeBonus, texture);
    menuButtons.push_back(button);

    auto newGameTexture = tEditor.createMenuButton("NEW GAME", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    rect.y += 175;
    button = std::make_shared<Button>(this, rect, newGameTexture, &Button::new_game_confirmation, arcadeButton99);
    menuButtons.push_back(button);

    texture = tEditor.createMenuButton("SETTINGS", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    rect.y += 175;
    button = std::make_shared<Button>(this, rect, texture, &Button::go_to_settings, arcadeButton99);
    menuButtons.push_back(button);

    texture = tEditor.createMenuButton("EXIT", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    rect.y += 175;
    button = std::make_shared<Button>(this, rect, texture, &Button::exit_to_desktop, arcadeButton99);
    menuButtons.push_back(button);


    rect.x = HIGHSCORE_CENTREPOS - (BUTTON_WIDTH/2);
    texture = tEditor.createMenuButton("RESET", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    button = std::make_shared<Button>(this, rect, texture, &Button::reset_highscores_confirmation, arcadeButton99);
    menuButtons.push_back(button);

    rect = {(wRect.w-BUTTON_WIDTH)/2, wRect.h/2, BUTTON_WIDTH, BUTTON_HEIGHT};
    button = std::make_shared<Button>(this, rect, newGameTexture, &Button::load_new_game, arcadeBonus);
    gameOverButtons.push_back(button);
}

void GameMenu::create_pauseMenu_buttons()
{
    SDL_Rect rect = {(wRect.w-BUTTON_WIDTH)/2, 128, BUTTON_WIDTH, BUTTON_HEIGHT};

    auto texture = tEditor.createMenuButton("RESUME", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);

    auto button = std::make_shared<Button>(this, rect, texture, &Button::close_pause_menu, arcadeBonus);
    pauseButtons.push_back(button);

    auto MenuTexture = tEditor.createMenuButton("MAIN MENU", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic36);
    rect.y += 175;
    button = std::make_shared<Button>(this, rect, MenuTexture, &Button::exit_to_menu_confirmation, arcadeBonus);
    pauseButtons.push_back(button);


    rect.y = 256;
    texture = tEditor.createMenuButton("YES", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    button = std::make_shared<Button>(this, rect, texture, &Button::doNothing, arcadeBonus);
    confirmationButtons.push_back(button);

    rect.y += 175;
    texture = tEditor.createMenuButton("NO", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    button = std::make_shared<Button>(this, rect, texture, &Button::doNothing, arcadeButton99);
    confirmationButtons.push_back(button);

    rect = {(wRect.w-BUTTON_WIDTH)/2, (wRect.h/2)+175, BUTTON_WIDTH, BUTTON_HEIGHT};
    button = std::make_shared<Button>(this, rect, MenuTexture, &Button::go_to_main_menu_from_gameover, arcadeButton99);
    gameOverButtons.push_back(button);
}

void GameMenu::create_settings_buttons()
{
    int x = wRect.w - (9 * BUTTON_WIDTH/8), y = 64;
    SDL_Rect rect = {x, y, BUTTON_WIDTH, BUTTON_HEIGHT};

    auto texture = tEditor.createMenuButton("REVERT", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    auto button = std::make_shared<Button>(this, rect, texture, &Button::revert_settings, arcadeButton99);
    settingsButtons.push_back(button);
    rect.y += 175;

    texture = tEditor.createMenuButton("RESET", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    button = std::make_shared<Button>(this, rect, texture, &Button::reset_settings, arcadeButton99);
    settingsButtons.push_back(button);
    rect.y += 175;

    texture = tEditor.createMenuButton("RETURN", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    button = std::make_shared<Button>(this, rect, texture, &Button::go_to_mainMenu, arcadeBonus);
    settingsButtons.push_back(button);



    rect = {64, 508, CHECKBOX_SIDELENGTH, CHECKBOX_SIDELENGTH};
    
    auto unselected = std::make_shared<LTexture>(window),
         selected   = std::make_shared<LTexture>(window);
    if (!unselected->loadFromFile("../../assets/Menu/Buttons/UnselectedCheckbox.png")) {
        std::cerr << "Failed to load unselected checkbox!" << std::endl;
    }
    if (!selected->loadFromFile("../../assets/Menu/Buttons/SelectedCheckbox.png")) {
        std::cerr << "Failed to load selected checkbox!" << std::endl;
    }
    button = std::make_shared<Button>(this, rect, unselected, &Button::toggle_CRT, arcadeButton99, selected);
    settingsButtons.push_back(button);
    if (settings.flags&CRT_FILTER) button->swap_textures();

    rect.y += CHECKBOX_SIDELENGTH + 16;
    button = std::make_shared<Button>(this, rect, unselected, &Button::toggle_fullscreen, arcadeButton99, selected);
    settingsButtons.push_back(button);
    if (settings.flags&FULLSCREEN) button->swap_textures();
    
    rect.y += CHECKBOX_SIDELENGTH + 16;
    button = std::make_shared<Button>(this, rect, unselected, &Button::toggle_FPS, arcadeButton99, selected);
    settingsButtons.push_back(button);
    if (settings.flags&SHOW_FPS) button->swap_textures();
    
    rect.y = 852; rect.x = 512;
    button = std::make_shared<Button>(this, rect, unselected, &Button::toggle_instructions, arcadeButton99, selected);
    settingsButtons.push_back(button);
    if (settings.flags&SHOW_INSTRUCTIONS) button->swap_textures();

    rect = {64, 820, BUTTON_WIDTH/2, BUTTON_HEIGHT/2};
    auto fpsbutton = std::make_shared<FPSButton>(-1, this, rect, &Button::doNothing, arcadeButton99);
    settingsButtons.push_back(fpsbutton);

    rect.y += rect.h + 16;
    fpsbutton = std::make_shared<FPSButton>(120, this, rect, &Button::doNothing, arcadeButton99);
    settingsButtons.push_back(fpsbutton);

    rect.x += rect.w + 16;
    fpsbutton = std::make_shared<FPSButton>(144, this, rect, &Button::doNothing, arcadeButton99);
    settingsButtons.push_back(fpsbutton);

    rect.y -= rect.h + 16;
    fpsbutton = std::make_shared<FPSButton>(60, this, rect, &Button::doNothing, arcadeButton99);
    settingsButtons.push_back(fpsbutton);
    

    int minX = 72, maxX = 504;
    float t = (float)settings.volume / MIX_MAX_VOLUME;
    x = (minX * (1.0f - t)) + (maxX * t);

    rect = {x, 169, SLIDER_WIDTH, SLIDER_HEIGHT};
    texture = tEditor.createSliderTexture(SLIDER_WIDTH, SLIDER_HEIGHT, window, {255,255,255,255});
    button = std::make_shared<Button>(this, rect, texture, &Button::volume_slider);
    settingsButtons.push_back(button);

    t = (float)settings.musicVolume / MIX_MAX_VOLUME;
    rect.x = (minX * (1.0f - t)) + (maxX * t);

    rect.y = 341 - SLIDER_HEIGHT/2;
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
    arcadeBonus = std::make_shared<LAudio>();
    if (!arcadeBonus->loadFromFile("../../assets/Audio/Menu/arcade-bonus.wav")) {
        std::cerr << "Failed to load button sound" << std::endl;
    }
    arcadeButton99 = std::make_shared<LAudio>();
    if (!arcadeButton99->loadFromFile("../../assets/Audio/Menu/arcadebutton99.wav")) {
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
    SDL_Rect rect = { 0, 0, int(CRT_Base->getWidth()*PIXEL_RENDER_SCALE), int(CRT_Base->getHeight()*PIXEL_RENDER_SCALE) };
    int nx = CRT_Tex->getWidth() / rect.w, ny = CRT_Tex->getHeight() / rect.h;

    for (int i = 0; i <= nx; i++) {
        int x = i * rect.w;
        for (int j = 0; j <= ny; j++) {
            int y = j * rect.h;
            rect.x = x; rect.y = y;
            tEditor.renderTextureToTexture(CRT_Tex, CRT_Base, &rect);
        }
    }
    CRT_Tex->setAlpha(PIXEL_FILTER_ALPHA);
}