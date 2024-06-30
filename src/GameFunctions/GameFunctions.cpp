#include "GameFunctions.hpp"

// initialises the game
GAME::GAME(std::shared_ptr<SDLHolder> g_Holder) : gHolder(g_Holder)
{
    // set up the background texture
    BGTexture = std::make_shared<LTexture>(gHolder);
    // Load background texture
    if (!BGTexture->loadFromFile("../assets/bg.png")) {
        printf("Failed to load background texture!\n");
    }
    std::cout << BGTexture->getWidth() <<' '<< BGTexture->getHeight() <<'\n';


    // instantiate a player
    player = Instantiate(Player, Vector2(100,100), 5, &gameObjects);
}

GAME::GAME(const GAME &) {}



// accessors
std::shared_ptr<SDLHolder> GAME::get_SDLHolder() { return gHolder; }





// takes in user input
void GAME::handleEvent( SDL_Event& e )
{
    switch (e.type)
    {
        case SDL_KEYDOWN:
            // a key was pressed
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
                    case SDLK_LSHIFT:
                        inputKeys |= 16; break;
                    case SDLK_F9:
                        debugging_tools ^= 1; break;
                    case SDLK_F8:
                        debugging_tools ^= 2; break;
                    case SDLK_F6:
                        debugging_tools ^= 4; break;
                }
            }

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
                    case SDLK_LSHIFT:
                        inputKeys &= ~16; break;
                }
            }
    }
}



void GAME::set_deltaTime( LTimer timer ) {
    Uint32 curr_time = timer.getTicks();
    // find the number of ticks elapsed
    dTime = curr_time - begin_time;
    
    // update begin_time for future calculations
    begin_time = curr_time;
}

Uint32 GAME::deltaTime() { return dTime; }



unsigned char GAME::get_inputs() { return inputKeys; }
unsigned char GAME::get_debugTools() { return debugging_tools; }


// updates all the game objects
void GAME::update_gameObjects() {
    for (int i = 0; i < gameObjects.size(); i++) {
        gameObjects[i]->update();
    }
}

// renders all game objects
void GAME::render_gameObjects() {
    for (int i = 0; i < gameObjects.size(); i++) {
        gameObjects[i]->render( camera.x, camera.y );
    }
}


// renders the background texture
void GAME::render_background() { BGTexture->render(0, 0, &camera); }


// moves the camera to the player
void GAME::moveCameraToPlayer() {
    // position of the player, size of the hitbox
    Vector2 playerPos = player->get_pos(), size = player->get_size();
    // dimensions of the camera rect
    Vector2 camDimensions(camera.w, camera.h);

    // camPos = (playerPos + playerSize/2) - camDimensions/2 = playerPos + (size - dimensions)/2
    Vector2 camPos = playerPos + (size - camDimensions) / 2;

    camera.x = camPos.x; camera.y = camPos.y;
}



// for instantiating game objects. returns a reference to the object created
std::shared_ptr<GameObject> GAME::Instantiate( EntityType type, Vector2 pos, int hp, std::vector<std::shared_ptr<GameObject>> *vec )
{
    auto obj = std::make_shared<GameObject>(pos, Vector2_Zero, type, hp, vec->size(), this);
    vec->push_back(obj);
    return obj;
}



// deletes the specified object
void Destroy( std::shared_ptr<GameObject> obj, std::vector<std::shared_ptr<GameObject>> *vec )
{
    // decrement the indices of all game objects following the specified one
    for (int i = obj->get_idx(); i < vec->size(); i++)    (*vec)[i]->decrement_idx();
    // delete the object itself
    obj->Delete();
}