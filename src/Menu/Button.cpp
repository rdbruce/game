#include "Button.hpp"
#include "Menu.hpp"

Button::Button( GameMenu *Menu, SDL_Rect Rect, std::shared_ptr<LTexture> Tex, void (Button::*Func)(), std::shared_ptr<LTexture> AltTex )
: menu(Menu), rect(Rect), tex(Tex), func(Func), altTex(AltTex) {}

bool Button::isPressed( int x, int y )
{
    int r = rect.x + rect.w, b = rect.y + rect.h;
    return x>=rect.x && x<=r && y>=rect.y && y<=b;
}

void Button::render( int dx, int dy )
{
    tex->render(rect.x + dx, rect.y + dy, &rect);
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

bool Button::is_toggled() { return isToggled; }

void Button::doNothing() {}

void Button::volume_slider()
{
    int x, y;
    menu->get_mousePos(&x, &y);
    x -= rect.w/2;

    int minX = 72, maxX = 504;

    x = Clamp(minX, maxX, x);
    rect.x = x;

    // find and set the new global volume
    float t = float(x - minX) / (maxX - minX);
    int newVolume = MIX_MAX_VOLUME * t;
    menu->settings.volume = newVolume;
    Mix_Volume(-1, newVolume);
}

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
    menu->game->scores = menu->highscores[0];
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
    menu->save_highscores();
    srand(time(NULL));
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
    menu->highscores[0] = menu->game->scores;
}

void Button::go_to_main_menu_from_gameover()
{
    menu->highscores[0] = menu->game->scores;
    int idx = menu->new_highscore();

    if (idx != 0) 
    {
        PlayerData newHighscore = menu->highscores[0];
        newHighscore.name = "___";
        menu->highscores[idx] = newHighscore;
        menu->set_score_name = idx;
        menu->confirmationText = "";
    }

    go_to_mainMenu();
}

void Button::reset_highscores_confirmation()
{
    auto yes = menu->confirmationButtons[0], no = menu->confirmationButtons[1];

    yes->set_func(&Button::reset_highscores);
    no->set_func(&Button::go_to_mainMenu);

    menu->confirmationText = "This will permanently erase\nall highscores!";
    menu->currButtons = &menu->confirmationButtons;
}

void Button::reset_highscores()
{
    int n = menu->num_highscores;
    for (int i = 1; i <= n; i++) {
        menu->highscores[i].reset();
    }
    go_to_mainMenu();
}

void Button::go_to_main_menu_from_settings()
{
    menu->settings.Save("../../saves/data/UserSettings.txt");
    go_to_mainMenu();
}

void Button::go_to_settings()
{
    menu->prevSettings = menu->settings;
    menu->state = settings_menu;
    menu->isActive = true;
    menu->currButtons = &menu->settingsButtons;
    menu->confirmationText = "";
}

void Button::revert_settings()
{
    menu->settings = menu->prevSettings;
    apply_settings();
}

void Button::reset_settings()
{
    menu->settings.reset();
    apply_settings();
}

void Button::apply_settings()
{
    Mix_Volume(-1, menu->settings.volume);

    int idx = menu->settingsButtons.size() - 1;
    auto slider = menu->settingsButtons[idx];

    int minX = 72, maxX = 504;
    float t = (float)menu->settings.volume / MIX_MAX_VOLUME;
    int x = (minX * (1.0f - t)) + (maxX * t);

    int X, Y;
    slider->get_pos(&X, &Y);
    slider->set_pos(x, Y);

    idx -= 1;
    auto checkbox = menu->settingsButtons[idx];
    if (bool(menu->settings.flags&SHOW_FPS) != checkbox->is_toggled()) checkbox->swap_textures();

    idx -= 1;
    checkbox = menu->settingsButtons[idx];
    if (menu->fullscreen != bool(menu->settings.flags&FULLSCREEN)) 
    {
        menu->fullscreen = menu->window->toggleFullscreen();
        menu->sizeChange++;
        if (menu->state == in_game || menu->state == game_over) {
            menu->game->initialise_BGTexture();
        }
        menu->settings.flags &= ~FULLSCREEN;
        menu->settings.flags |= menu->fullscreen << 4;
    }
    if (menu->fullscreen != checkbox->is_toggled()) checkbox->swap_textures();

    idx -= 1;
    checkbox = menu->settingsButtons[idx];
    if (bool(menu->settings.flags&CRT_FILTER) != checkbox->is_toggled()) checkbox->swap_textures();
}

void Button::get_pos( int *x, int *y )
{
    *x = rect.x; *y = rect.y;
}

void Button::swap_textures()
{
    auto temp = tex;
    tex = altTex;
    altTex = temp;
    isToggled = !isToggled;
}

void Button::toggle_CRT()
{
    menu->settings.flags ^= CRT_FILTER;
    swap_textures();
}

void Button::toggle_fullscreen()
{
    menu->fullscreen = menu->window->toggleFullscreen();
    menu->sizeChange++;
    if (menu->state == in_game || menu->state == game_over) {
        menu->game->initialise_BGTexture();
    }
    menu->settings.flags &= ~FULLSCREEN;
    menu->settings.flags |= menu->fullscreen << 4;
    swap_textures();
}

void Button::toggle_FPS()
{
    menu->settings.flags ^= SHOW_FPS;
    swap_textures();
}