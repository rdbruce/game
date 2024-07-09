#pragma once

#include "../engine/LTexture.hpp"
#include "../engine/LWindow.hpp"

#include <iostream>

class GameMenu
{
    public:

        // initialise menu
        GameMenu( std::shared_ptr<LWindow> Window );

        // renders the background texture
        void render_background();

        // handle user input
        void handle_events( SDL_Event &e, bool *menuActive );

    private:

        std::shared_ptr<LWindow> window;

        std::shared_ptr<LTexture> BGTexture = nullptr;

        bool isActive = true;
};