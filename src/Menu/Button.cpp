#include "Button.hpp"
#include "Menu.hpp"

Button::Button( GameMenu *Menu, SDL_Rect Rect, std::shared_ptr<LTexture> Tex, void (Button::*Func)() )
: menu(Menu), rect(Rect), tex(Tex), func(Func) {}

bool Button::isPressed( int x, int y )
{
    int r = rect.x + rect.w, b = rect.y + rect.h;
    return x>=rect.x && x<=r && y>=rect.y && y<=b;
}

void Button::render()
{
    tex->render(rect.x, rect.y, &rect);
}

void Button::execute_function() {
    (this->*func)();
}

void Button::doNothing() {}

void Button::enter_game()
{
    menu->state = in_game;
    menu->isActive = false;
    menu->currButtons = &menu->pauseButtons;
}

void Button::continue_game()
{
    menu->game->load_levels("../../saves/curr/");
    menu->game->initialise_BGTexture();
    enter_game();
}

void Button::load_new_game()
{
    menu->game->load_levels("../../saves/NewGame/");
    menu->game->initialise_BGTexture();
    // save the game to overwrite any existing save data
    menu->game->save_game();
    enter_game();
}

void Button::exit_to_desktop()
{
    menu->state = Quit;
}

void Button::close_pause_menu()
{
    menu->isActive = false;
    menu->game->clear_input();
}

void Button::go_to_mainMenu()
{
    menu->state = main_menu;
    menu->isActive = true;
    menu->currButtons = &menu->menuButtons;
}