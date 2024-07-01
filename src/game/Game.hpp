#pragma once

#include "../engine/LTexture.hpp"
#include "../engine/LTimer.hpp"
#include "../engine/LWindow.hpp"
#include "../engine/GameMath.hpp"
#include "../gameObject/GameObject.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_events.h>

#include <ctime>

using namespace Math;


class Game
{
    // declare GameObject as a friend of Game, so the private members can be accessed
    friend class GameObject;

    public:

        // constructor
        Game( std::shared_ptr<LWindow> Window );


        // handle events like player input
        void handle_events( SDL_Event& e );



        // updates all the game objects
        void update_gameobjects();


        // rendering functions

        // renders the scene's background
        void render_background();

        // renders all the game objects
        void render_gameobjects();


        // finds the time elapsed between frames
        void update_deltaTime();


    private:



        // for tracking player input
        // tracks which keys the player has held down
        Uint8 inputKeys = 0;



        // dimensions of the current map
        Vector2Int mapDimensions;


        // the time elapsed between frames
        float deltaTime = 0.0f;
        clock_t begin_time = 0; // used for calculating deltaTime




        // position and dimensions of the camera
        SDL_Rect camera;


        // pointers to game objects
        std::shared_ptr<GameObject> player = nullptr;

        

        // images/textures

        // background texture
        std::shared_ptr<LTexture> BGTexture = nullptr;
        
        // the window the game will be rendered to
        std::shared_ptr<LWindow> window = nullptr;
};