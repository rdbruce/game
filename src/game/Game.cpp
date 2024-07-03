#include "Game.hpp"

// constructor
Game::Game( std::shared_ptr<LWindow> Window ) : window(Window)
{
    // load the background texture
    Bert = std::make_shared<LTexture>(window);
    if (!Bert->loadFromFile("../../assets/Bert.png")) {
        printf("Failed to load background texture!\n");
    }

    load_textures();

    // camera dimensions should be the same as window size
    camera = { 0, 0, window->getWidth(), window->getHeight() };


    // load the test level
    testLevel = Scene("../../saves/test.txt", this);
    currLevel = &testLevel;

    initialise_BGTexture();
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
    for (int i = 0; i < currLevel->gameObjects.size(); i++) {
        currLevel->gameObjects[i]->update();
    }
}




void Game::center_camera_on_player()
{
    Vector2 pPos = currLevel->player->get_pos(),
            cPos = pPos - Vector2(camera.w/2, camera.h/2);

    // if the camera is on the edge of the map, snap it back in bounds
    cPos.x = clampf(0.0f, map.w-camera.w, cPos.x);
    cPos.y = clampf(0.0f, map.h-camera.h, cPos.y);

    camera.x = cPos.x; camera.y = cPos.y;
}


// renders the scene's background
void Game::render_background() {
    BGTexture->render(0, 0, &camera, &camera);
}



// renders all the game objects
void Game::render_gameobjects() {
    int n = currLevel->gameObjects.size();
    for (int i = 0; i < n; i++) {
        currLevel->gameObjects[i]->render( camera.x, camera.y );
    }
}

// finds the time elapsed between frames
void Game::update_deltaTime() {
    clock_t t = clock(); // current time
    deltaTime = float(t - begin_time) / CLOCKS_PER_SEC;
    begin_time = t;
}


Vector2 Game::find_mouse_pos()
{
    int x, y;
    // get the coordinates of the mouse on the screen
    SDL_GetMouseState(&x, &y);
    // convert that to coordinates in the game
    return Vector2(x+camera.x, y+camera.y);
}



// adds a game object and returns a pointer to it
std::shared_ptr<GameObject> Game::Instantiate( Vector2 pos, int type, int hp, Scene *level )
{
    // create the game object
    auto obj = std::make_shared<GameObject>(pos, (EntityType)type, (int)level->gameObjects.size(), hp, this, level->cell_sideLen);
    // add it to the game objects vector
    level->gameObjects.push_back(obj);
    return obj;
}


std::shared_ptr<GameObject> Game::Instantiate( Vector2 pos, int type, int hp ) {
    return Instantiate( pos, type, hp, currLevel );
}


void Game::Destroy( std::shared_ptr<GameObject> obj, std::vector<std::shared_ptr<GameObject>> *vec )
{
    if (obj == nullptr) return;

    int n = obj->get_idx();
    // decrement the indices of all following game objects,
    // as the size of the vector will decrease
    for (int i = n+1; i < vec->size(); i++) (*vec)[i]->decrement_idx();

    // erase the object from the vector
    vec->erase( vec->begin() + n );

    // delete the object from memory
    obj->~GameObject();
}
void Game::Destroy( std::shared_ptr<GameObject> obj ) {
    Destroy( obj, &currLevel->gameObjects );
}


std::shared_ptr<GameObject> Game::spawnItemStack( int type, Vector2 pos, int count )
{
    // only items will be spawned this way
    if (!is_item(type)) return nullptr;

    int num = RAND_MAX/2;
    Vector2 dir(rand()-num, rand()-num);

    // normalise dir to get a unit vector direction
    dir.normalise();

    auto obj = Instantiate(pos, type, count);

    obj->set_velocity( dir * 35.0f );
    obj->set_acceleration( dir * -35.0f );

    return obj;
}


std::shared_ptr<GameObject> Game::craftTwoItems( std::shared_ptr<GameObject> item1, std::shared_ptr<GameObject> item2 )
{
    // come back here !!
    std::shared_ptr<GameObject> res = nullptr;
    int hp1 = item1->get_hp(), hp2 = item2->get_hp();

    switch (item1->get_type())
    {
        case Log_Item:
            switch (item2->get_type())
            {
                case Plank_Item:
                    // 2 logs and 4 planks craft one bridge
                    // make sure there are adequate resources
                    if (hp1 >= 2 && hp2 >= 4) {
                        // spawn one bridge item
                        res = spawnItemStack(Bridge_Item, item1->get_pos(), 1);
                        // reduce the hp of the other item stacks
                        item1->set_HP(hp1-2); item2->set_HP(hp2-4);
                    }
                    break;  
            }
            break;

        case Plank_Item:
            // 2 logs and 4 planks craft one bridge
            // make sure there are adequate resources
            if (hp1 >= 4 && hp2 >= 2) {
                // spawn one bridge item
                res = spawnItemStack(Bridge_Item, item1->get_pos(), 1);
                // reduce the hp of the other item stacks
                item1->set_HP(hp1-4); item2->set_HP(hp2-2);
            }
            break;
    }
    return res;
}


void Game::movePlayerToLevel( Scene *level, Vector2 newPlayerPos )
{
    // come back here !!

    level->player = Instantiate(newPlayerPos, Player, currLevel->player->get_hp(), level);
    Destroy(currLevel->player);
    currLevel->player = nullptr;
    currLevel = level;
}


std::shared_ptr<GameObject> Game::moveEntityToLevel( std::shared_ptr<GameObject> obj, Scene *level, Vector2 newPos )
{
    // come back here !!

    auto res = Instantiate(newPos, obj->get_type(), obj->get_hp(), level);
    Destroy(obj);
    return res;
}



bool Game::is_item( int type ) { return type >= Log_Item; }


void Game::load_textures()
{
    // load all the textures from their image files

    grassTex = std::make_shared<LTexture>(window);
    if (!grassTex->loadFromFile("../../assets/Ground/Grass.png")) {
        std::cerr << "Failed to load grass texture!" << std::endl;
    }

    logTex = std::make_shared<LTexture>(window);
    if (!logTex->loadFromFile("../../assets/Buildings/Log.png")) {
        std::cerr << "Failed to load log texture!" << std::endl;
    }

    waterTex = std::make_shared<LTexture>(window);
    if (!waterTex->loadFromFile("../../assets/Ground/Water.png")) {
        std::cerr << "Failed to load water texture!" << std::endl;
    }

    treeTex = std::make_shared<LTexture>(window);
    if (!treeTex->loadFromFile("../../assets/Tree/Tree.png")) {
        std::cerr << "Failed to load tree texture!" << std::endl;
    }

    stumpTex = std::make_shared<LTexture>(window);
    if (!stumpTex->loadFromFile("../../assets/Tree/Stump.png")) {
        std::cerr << "Failed to load stump texture!" << std::endl;
    }

    saplingTex = std::make_shared<LTexture>(window);
    if (!saplingTex->loadFromFile("../../assets/Tree/Sapling.png")) {
        std::cerr << "Failed to load sapling texture!" << std::endl;
    }

    shoreline0Tex = std::make_shared<LTexture>(window);
    if (!shoreline0Tex->loadFromFile("../../assets/Ground/Shoreline0.png")) {
        std::cerr << "Failed to load shoreline 0 texture!" << std::endl;
    }
    shoreline1Tex = std::make_shared<LTexture>(window);
    if (!shoreline1Tex->loadFromFile("../../assets/Ground/Shoreline1.png")) {
        std::cerr << "Failed to load shoreline 1 texture!" << std::endl;
    }
    shoreline2Tex = std::make_shared<LTexture>(window);
    if (!shoreline2Tex->loadFromFile("../../assets/Ground/Shoreline2.png")) {
        std::cerr << "Failed to load shoreline 2 texture!" << std::endl;
    }
    shoreline3Tex = std::make_shared<LTexture>(window);
    if (!shoreline3Tex->loadFromFile("../../assets/Ground/Shoreline3.png")) {
        std::cerr << "Failed to load shoreline 3 texture!" << std::endl;
    }
    shoreline4Tex = std::make_shared<LTexture>(window);
    if (!shoreline4Tex->loadFromFile("../../assets/Ground/Shoreline4.png")) {
        std::cerr << "Failed to load shoreline 4 texture!" << std::endl;
    }
    closed_doorTex = std::make_shared<LTexture>(window);
    if (!closed_doorTex->loadFromFile("../../assets/Buildings/Door.png")) {
        std::cerr << "Failed to load closed door texture!" << std::endl;
    }
    open_doorTex = std::make_shared<LTexture>(window);
    if (!open_doorTex->loadFromFile("../../assets/Buildings/Open_Door.png")) {
        std::cerr << "Failed to load open door texture!" << std::endl;
    }
    CRT_Tex = std::make_shared<LTexture>(window);
    if (!CRT_Tex->loadFromFile("../../assets/CRT_Base_Texture.png")) {
        std::cerr << "Failed to load CRT texture!" << std::endl;
    }
}