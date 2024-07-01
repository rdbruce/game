#include "Game.hpp"

// constructor
Game::Game( std::shared_ptr<LWindow> Window ) : window(Window)
{
    // load the background texture
    BGTexture = std::make_shared<LTexture>(window);
    if (!BGTexture->loadFromFile("../../assets/Bert.png")) {
        printf("Failed to load background texture!\n");
    }


    // camera dimensions should be the same as window size
    camera = { 0, 0, window->getWidth(), window->getHeight() };


    // initialise the player
    player = std::make_shared<GameObject>( Vector2_Zero, this );
}


// handle events like player input
void Game::handle_events( SDL_Event& e )
{
    switch (e.type)
    {
        case SDL_KEYDOWN:
            if (e.key.repeat == 0) {
                switch (e.key.keysym.sym)
                {
                    case SDLK_w:
                        inputKeys |= 8; break;
                    case SDLK_a:
                        inputKeys |= 4; break;
                    case SDLK_s:
                        inputKeys |= 2; break;
                    case SDLK_d:
                        inputKeys |= 1; break;
                }
            }
            break;


        case SDL_KEYUP:
            if (e.key.repeat == 0) {
                switch (e.key.keysym.sym)
                {
                    case SDLK_w:
                        inputKeys &= ~8; break;
                    case SDLK_a:
                        inputKeys &= ~4; break;
                    case SDLK_s:
                        inputKeys &= ~2; break;
                    case SDLK_d:
                        inputKeys &= ~1; break;
                }
            }
            break;
    }
}



// updates all the game objects
void Game::update_gameobjects()
{
    player->update();
}





// renders the scene's background
void Game::render_background() {
    BGTexture->renderAsBackground();
}



// renders all the game objects
void Game::render_gameobjects() {
    player->render( camera.x, camera.y );
}

// finds the time elapsed between frames
void Game::update_deltaTime() {
    clock_t t = clock(); // current time
    deltaTime = float(t - begin_time) / CLOCKS_PER_SEC;
    begin_time = t;
}