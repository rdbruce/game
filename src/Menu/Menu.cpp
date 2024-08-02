#include "Menu.hpp"
#include "Button.hpp"

GameMenu::GameMenu( std::shared_ptr<LWindow> Window, Game *game ) 
: window(Window), game(game)
{
    load_assets();

    // create buttons
    create_mainMenu_buttons();
    create_pauseMenu_buttons();

    // load player highscores
    load_highscores();
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

void GameMenu::render_highscores()
{
    if (state == main_menu && confirmationText == "") 
    {
        SDL_Rect rect = {25, 25, 400, 240};

        renderText("HIGHSCORES", rect.x+(rect.w/2), rect.y, window, {255,0,0,255}, sevenSegment36);
        rect.y += 55;

        int x = rect.x + 160, y = rect.y;

        renderText("SCORE", x, y, window, {255,0,0,255}, sevenSegment24); x += 75;
        renderText("NIGHSTS\nSURVIVED", x, y, window, {255,0,0,255}, sevenSegment18); x += 75;
        renderText("ENEMIES\nKILLED", x, y, window, {255,0,0,255}, sevenSegment18);

        rect.y += 55;

        for (int i = 1; i <= num_highscores; i++)
        {
            if (set_score_name == i) 
            {
                highscores[i].render(rect.x, rect.y, window, {255,255,255,255}, sevenSegment24, currChar, {255,0,0,255});
                x = rect.x + 425;
                std::string str = "NEW HIGHSCORE!";
                renderText(str, rect.x, rect.y, window, {255,0,0,255}, sevenSegment24, Left_aligned);
            } 
            else 
            {
                highscores[i].render(rect.x, rect.y, window, {255,255,255,255}, sevenSegment24);
            }
            rect.y += 45;
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

void GameMenu::render_CRT()
{
    CRT_Tex->renderAsBackground();
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

                default:
                    if (set_score_name != 0) rename_highscore(e.key.keysym.sym);
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT) leftClickFunc();
            break;
    }
    *menuActive = isActive;

    update();

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

void GameMenu::update()
{
    if (state == Quit) {
        save_highscores();
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
    
    int x = window->getWidth() - 325, y = 175;
    SDL_Rect rect = {x, y, 262, 62};
    auto button = std::make_shared<Button>(this, rect, texture, &Button::continue_game);
    menuButtons.push_back(button);
    

    auto newGameTexture = std::make_shared<LTexture>(window);
    if (!newGameTexture->loadFromFile("../../assets/Menu/Buttons/NewGameButton.png")) {
        std::cerr << "Failed to load new game button texture!" << std::endl;
    }
    rect.y += 87;
    button = std::make_shared<Button>(this, rect, newGameTexture, &Button::new_game_confirmation);
    menuButtons.push_back(button);

    texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/SettingsButton.png")) {
        std::cerr << "Failed to load settings button texture!" << std::endl;
    }
    rect.y += 87;
    button = std::make_shared<Button>(this, rect, texture);
    menuButtons.push_back(button);

    texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/ExitButton.png")) {
        std::cerr << "Failed to load texture for exit button" << std::endl;
    }
    rect.y += 87;
    button = std::make_shared<Button>(this, rect, texture, &Button::exit_to_desktop);
    menuButtons.push_back(button);


    rect.w *= 0.9; rect.h *= 0.9; rect.x = 212-(rect.w/2);
    texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/ResetButton.png")) {
        std::cerr << "failed to load reset button texture!" << std::endl;
    }
    button = std::make_shared<Button>(this, rect, texture, &Button::reset_highscores_confirmation);
    menuButtons.push_back(button);

    rect = {(window->getWidth()/2) - 131, 300, 262, 62};
    button = std::make_shared<Button>(this, rect, newGameTexture, &Button::load_new_game);
    gameOverButtons.push_back(button);
}

void GameMenu::create_pauseMenu_buttons()
{
    int x = (window->getWidth()/2) - 131, y = 125;
    SDL_Rect rect = {x, y, 262, 62};

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
    rect.y += 87;
    button = std::make_shared<Button>(this, rect, MenuTexture, &Button::exit_to_menu_confirmation);
    pauseButtons.push_back(button);


    rect.y = 175;
    texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/YesButton.png")) {
        std::cerr << "Failed to load texture for yes button!" << std::endl;
    }
    button = std::make_shared<Button>(this, rect, texture);
    confirmationButtons.push_back(button);

    rect.y += 87;
    texture = std::make_shared<LTexture>(window);
    if (!texture->loadFromFile("../../assets/Menu/Buttons/NoButton.png")) {
        std::cerr << "Failed to load texture for no button!" << std::endl;
    }
    button = std::make_shared<Button>(this, rect, texture);
    confirmationButtons.push_back(button);

    rect.y = 425;
    button = std::make_shared<Button>(this, rect, MenuTexture, &Button::go_to_main_menu_from_gameover);
    gameOverButtons.push_back(button);
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

    sevenSegment18 = TTF_OpenFont("../../assets/Fonts/Seven_Segment.ttf", 18);
    if (sevenSegment18 == NULL) {
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


    auto CRT_Base = std::make_shared<LTexture>(window);
    if (!CRT_Base->loadFromFile("../../assets/CRT_Base_Texture.png")) {
        std::cerr << "Failed to load CRT Pixel texture!" << std::endl;
    }

    CRT_Tex = tEditor.createEmptyTexture(window->getWidth(), window->getHeight(), window);
    int nx = CRT_Tex->getWidth() / CRT_Base->getWidth(), ny =( CRT_Tex->getHeight() / CRT_Base->getWidth())+1;

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