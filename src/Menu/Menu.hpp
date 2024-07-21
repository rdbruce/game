#pragma once

#include "../engine/LTexture.hpp"
#include "../engine/LWindow.hpp"
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

        // handle user input, returns true when quit is requested
        bool handle_events( SDL_Event &e, bool *menuActive );

        bool is_active();

        // creates all the button objects for the main menu
        void create_mainMenu_buttons();

        // creates all the button objects for the in-game pause menu
        void create_pauseMenu_buttons();

    private:

        std::shared_ptr<LWindow> window;

        std::shared_ptr<LTexture> BGTexture = nullptr;


        std::vector<std::shared_ptr<Button>> *currButtons = &menuButtons;
        std::vector<std::shared_ptr<Button>> menuButtons, // buttons in the main menu
                                             pauseButtons; // pause menu

        bool isActive = true;
        State state = main_menu;

        Game *game = nullptr;


        // checks to see if the player clicks any buttons
        void leftClickFunc();
};