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

void Button::set_func( void (Button::*newFunc)() ) {
    func = newFunc;
}

void Button::set_pos( int x, int y ) {
    rect.x = x; rect.y = y;
}

void Button::execute_function() {
    (this->*func)();
}

void Button::doNothing() {}

void Button::enter_game()
{
    menu->game->new_game();
    menu->game->clear_input();
    menu->state = in_game;
    menu->isActive = false;
    menu->currButtons = &menu->pauseButtons;
    menu->confirmationText = "";
}

void Button::continue_game()
{
    menu->game->load_levels("../../saves/curr/");
    menu->game->initialise_BGTexture();
    menu->game->scores.loadFromFile("../../saves/data/CurrScores.txt");
    enter_game();
}

void Button::new_game_confirmation()
{
    auto yes = menu->confirmationButtons[0], no = menu->confirmationButtons[1];

    yes->set_func(&Button::load_new_game);
    no->set_func(&Button::go_to_mainMenu);

    menu->confirmationText = "This will erase your previous save!";
    menu->currButtons = &menu->confirmationButtons;
}

void Button::load_new_game()
{
    menu->game->load_levels("../../saves/NewGame/");
    menu->game->initialise_BGTexture();
    // save the game to overwrite any existing save data
    menu->game->save_game();
    menu->game->scores.reset();
    menu->game->scores.Save("../../saves/data/CurrScores.txt");
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

void Button::exit_to_menu_confirmation()
{
    auto yes = menu->confirmationButtons[0], no = menu->confirmationButtons[1];

    yes->set_func(&Button::go_to_mainMenu);
    no->set_func(&Button::exit_decline);

    float time = menu->game->get_time();
    int mins = int(time / 60.0f);

    menu->confirmationText = "The last save was " + std::to_string(mins) + " minutes ago!";
    menu->currButtons = &menu->confirmationButtons;
}

void Button::exit_decline()
{
    menu->currButtons = &menu->pauseButtons;
    menu->confirmationText = "";
}

void Button::go_to_mainMenu()
{
    menu->state = main_menu;
    menu->isActive = true;
    menu->currButtons = &menu->menuButtons;
    menu->confirmationText = "";
    menu->highscores.set_newHighscores(menu->game->scores);
    menu->game->scores.Save("../../saves/data/CurrScores.txt");
}