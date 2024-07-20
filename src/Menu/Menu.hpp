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

        // handle user input
        void handle_events( SDL_Event &e, bool *menuActive );

        bool is_active();

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