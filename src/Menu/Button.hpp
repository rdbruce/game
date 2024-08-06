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

        // updates execute func
        void set_func( void (Button::*newFunc)() );

        // changes position
        void set_pos( int x, int y );

        // returns true if the button is pressed
        bool isPressed( int x, int y );

        // does something when the button is pressed
        void execute_function();

        // renders the button
        void render( int dx, int dy );
        
        
        /* BELOW ARE A BUNCH OF DIFFERENT BUTTON FUNCTIONS */

        // default button function
        void doNothing();

        // enters the game
        void enter_game();

        // loads previous save file
        void continue_game();

        void new_game_confirmation();
        // loads a new game file
        void load_new_game();

        // closes the game
        void exit_to_desktop();

        // closes the pause menu
        void close_pause_menu();

        void exit_to_menu_confirmation();
        void exit_decline();
        // exits the game, and goes to the main menu
        void go_to_mainMenu();

        void reset_highscores_confirmation();
        void reset_highscores();

        void go_to_main_menu_from_gameover();

        // enter the settings menu
        void go_to_settings();

        // follow the mouse, adjust volume
        void volume_slider();
};
