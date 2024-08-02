#pragma once

#include "../engine/LTexture.hpp"
#include "../engine/LWindow.hpp"
#include "../engine/TextureManipulator.hpp"
#include "../engine/LAudio.hpp"
#include "../game/Game.hpp"

#include <iostream>
#include <vector>
#include <memory>

// forwards declaration
class Button;


// different menu states
enum State
{
    main_menu,
    in_game,
    Quit,
    game_over
};


class GameMenu
{
    friend class Button;

    public:

        // initialise menu
        GameMenu( std::shared_ptr<LWindow> Window, Game *game );


        // renders the background texture
        void render_background();
        // shows all the buttons
        void render_buttons();
        void render_confirmation();
        // shows the highscores on the main menu
        void render_highscores();
        // renders the CRT overlay effect
        void render_CRT();


        // handle user input, returns true when quit is requested
        bool handle_events( SDL_Event &e, bool *menuActive );

        bool is_active();

        bool is_inGame();

        // creates all the button objects for the main menu
        void create_mainMenu_buttons();

        // creates all the button objects for the in-game pause menu
        void create_pauseMenu_buttons();

        // updates some of the internal variables
        void update();

        void load_assets();

        void load_highscores();
        void save_highscores();

    private:

        std::shared_ptr<LWindow> window;

        std::shared_ptr<LTexture> BGTexture = nullptr, gameOverTex, CRT_Tex;

        // the sound made when you click a button
        std::shared_ptr<LAudio> buttonSound = nullptr;

        // fonts
        TTF_Font *sevenSegment18, *sevenSegment24, *sevenSegment36;


        std::string confirmationText = "";

        std::vector<std::shared_ptr<Button>> *currButtons = &menuButtons;
        std::vector<std::shared_ptr<Button>> menuButtons, // buttons in the main menu
                                             pauseButtons, // pause menu
                                             confirmationButtons, // yes/no
                                             gameOverButtons; // play again/main menu

        bool isActive = true;
        State state = main_menu;

        Game *game = nullptr;

        TextureManipulator tEditor;

        int num_highscores;
        int set_score_name = 0;
        int currChar = 0;
        // [0] = the data struct for the current game, all other are highscores
        PlayerData highscores[6];

        // checks to see if the player clicks any buttons
        void leftClickFunc();

        void enter_game_over();

        // returns the index of the highscore the latest game was better than
        int new_highscore();

        void rename_highscore( SDL_Keycode sym );
};