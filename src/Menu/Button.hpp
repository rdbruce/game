#pragma once

#include "../engine/LTexture.hpp"
#include <SDL.h>

// forward declaration
class GameMenu;
class Button;


class Button
{
    private:

        // posiiton and dimensions of the button
        SDL_Rect rect;

        // texture to be rendered
        std::shared_ptr<LTexture> tex = nullptr;

        // reference to the menu it's a part of
        GameMenu *menu = nullptr;

        // what happens when the button is pressed
        void (Button::*func)();


        
    public:

        Button( GameMenu *Menu, SDL_Rect Rect, std::shared_ptr<LTexture> Tex, void (Button::*Func)() = &Button::doNothing );

        // returns true if the button is pressed
        bool isPressed( int x, int y );

        // does something when the button is pressed
        void execute_function();

        // renders the button
        void render();
        
        
        /* BELOW ARE A BUNCH OF DIFFERENT BUTTON FUNCTIONS */

        // default button function
        void doNothing();

        // enters the game
        void enter_game();

        // loads previous save file
        void continue_game();

        // loads a new game file
        void load_new_game();
};
