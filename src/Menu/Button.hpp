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
        std::shared_ptr<LTexture> tex = nullptr, altTex = nullptr;

        // reference to the menu it's a part of
        GameMenu *menu = nullptr;

        // what happens when the button is pressed
        void (Button::*func)();

        bool isToggled = false;

        
    public:

        Button( GameMenu *Menu, SDL_Rect Rect, std::shared_ptr<LTexture> Tex, void (Button::*Func)() = &Button::doNothing, std::shared_ptr<LTexture> AltTex = nullptr );

        // updates execute func
        void set_func( void (Button::*newFunc)() );

        // changes position
        void set_pos( int x, int y );

        // gets x and y coords
        void get_pos( int *x, int *y );

        // returns true if the button is pressed
        bool isPressed( int x, int y );

        // does something when the button is pressed
        void execute_function();

        // renders the button
        void render( int dx, int dy );

        bool is_toggled();
        
        
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
        // goes to main menu, and saves settings to file
        void go_to_main_menu_from_settings();
        // sets settings back to what they were before editing
        void revert_settings();
        // resets settings to default
        void reset_settings();

        // applies user settings
        void apply_settings();

        // follow the mouse, adjust volume
        void volume_slider();

        // swaps texture with alttex
        void swap_textures();

        // toggles whether or not the CRT Texture is rendered
        void toggle_CRT();

        void toggle_fullscreen();

        void toggle_FPS();
};
