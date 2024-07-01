#pragma once

#include "../game/Game.hpp"
#include "../engine/GameMath.hpp"
#include "../engine/LTexture.hpp"
#include "../engine/LWindow.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include <iostream>


// forwards declaration of 'Game' class (i hate ts)
class Game;


using namespace Math;

class GameObject
{
    public:

        // constructor
        GameObject( Vector2 pos, Game *game );


        // updates member variables
        void update();


        // renders the object onto the window, relative to a camera
        void render( int camX, int camY );



    private:

        // position of the centre of the object's hitbox
        Vector2 pos;


        // how fast the object can move
        float moveSpeed = 0.0f;


        // position and size of the hitbox
        SDL_Rect hitbox;

        // the image to be drawn
        LTexture tex;

        // a reference to the game itself
        Game *game = nullptr;
};