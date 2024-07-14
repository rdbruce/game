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