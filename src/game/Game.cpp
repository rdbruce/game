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
    interactRange = 3.0f * currLevel->cell_sideLen;

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

        case SDL_MOUSEBUTTONDOWN:
            // left click
            if (e.button.button == SDL_BUTTON_LEFT) leftClickFunc();
            else if (e.button.button == SDL_BUTTON_RIGHT) rightClickFunc();
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

void Game::render_overlay() {
    overlayTexture->render(0, 0, &camera, &camera);
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

    if (obj->is_held()) currLevel->held = nullptr;

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


std::shared_ptr<GameObject> Game::craftItem( std::shared_ptr<GameObject> item )
{
    // cannot craft btyy right clicking a held item
    if (item->is_held()) return nullptr;
    std::shared_ptr<GameObject> res = nullptr;

    // which type of item will be created
    switch (item->get_type())
    {
        case Log_Item: // one log may be crafted into two planks
            // spawn a stack of two planks on the log stack
            res = spawnItemStack(Plank_Item, item->get_pos(), 2);
            res->set_timer( 0.75f );
            item->set_HP( item->get_hp()-1 ); // lose one log in crafting
            break;

        case Bridge_Item: {
            // bridges may be deconstructed back into 4 planks and 2 logs
            Vector2 p = item->get_pos();
            res = spawnItemStack(Log_Item, p, 2);
            res->set_timer( 0.75f );
            res = spawnItemStack(Plank_Item, p, 4);
            res->set_timer( 0.75f );
            item->set_HP( item->get_hp()-1 );
            break;
        }

        case Plank_Item: { // four planks may be crafted into one door
            int hp = item->get_hp();
            if (hp >= 4) {
                // spawn a door on the plank stack
                res = spawnItemStack(Door_Item, item->get_pos(), 1);
                res->set_timer( 0.75f );
                item->set_HP( hp-4 );
            }
            break;
        }

        case Door_Item: // doors may be deconstructed into four planks
            res = spawnItemStack(Plank_Item, item->get_pos(), 4);
            res->set_timer( 0.75f );
            item->set_HP( item->get_hp()-1 );
            break;
    }
    return res;
}


void Game::movePlayerToLevel( Scene *level, Vector2 newPlayerPos )
{
    // if the player is holding something, it should be removed from the current scene
    // and added to the new scene, as the held object
    if (currLevel->held != nullptr) {
        // revise using the same position as the player
        level->held = moveEntityToLevel(currLevel->held, level, newPlayerPos);
        currLevel->held = nullptr;
    }

    level->player = Instantiate(newPlayerPos, Player, currLevel->player->get_hp(), level);
    Destroy(currLevel->player);
    currLevel->player = nullptr;
    currLevel = level;
}


std::shared_ptr<GameObject> Game::moveEntityToLevel( std::shared_ptr<GameObject> obj, Scene *level, Vector2 newPos )
{
    // if the object being moved is the held object, do nothing since it should move
    // with the player, not on its own
    if (obj->is_held()) return nullptr;

    auto res = Instantiate(newPos, obj->get_type(), obj->get_hp(), level);
    Destroy(obj);
    return res;
}



bool Game::is_item( int type ) { return type >= Log_Item; }


bool Game::isInRegion( Vector2 p, SDL_Rect rect )
{
    float r = rect.x+rect.w, b = rect.y+rect.h;
    return p.x>=rect.x && p.x<=r && p.y>=rect.y && p.y<=b;
}

int Game::get_building( EntityType type )
{
    switch (type)
    {
        case Log_Item: return LOG;
        case Pine_Cone_Item: return SAPLING;
        case Bridge_Item: return BRIDGE;
        case Door_Item: return CLOSED_DOOR;
        default: return EMPTY;
    }
}


void Game::leftClickFunc()
{
    Vector2 mPos = find_mouse_pos();
    float r = (mPos - currLevel->player->get_pos()).length();

    // the cell the player clicked on
    float sideLen = currLevel->cell_sideLen;
    Vector2Int cell(mPos.x/sideLen, mPos.y/sideLen);
    // do something depending on the type of building in the clicked cell
    int val = currLevel->grid[cell.x][cell.y]&255;

    switch (val)
    {
        case 7: // clicked a closed door, open it
            if (r <= interactRange) PlaceObjectInCell(cell, OPEN_DOOR, true);
            break;

        case 8: // clicked an open door, close it
            if (r <= interactRange) PlaceObjectInCell(cell, CLOSED_DOOR, true);
            break;
        
        default: // didn't click anything important, interact with items
            // if an item is NOT held, the player may pick up an item stack
            if (currLevel->held == nullptr && r <= interactRange)
            {
                // check all the game objects, and see if any of them ARE items AND were clicked
                int n = currLevel->gameObjects.size();
                for (int i =0; i < n; i++) {
                    auto obj = currLevel->gameObjects[i];

                    if (obj->is_item() && isInRegion(mPos, obj->get_hitbox()))
                    {
                        // make the player pick up the object and exit the function
                        setHeldObject(obj);
                        return;
                    }
                }
            // if an item IS held, throw it :)
            } else if (currLevel->held != nullptr) throwHeldObject();
            break;

    }
}


void Game::rightClickFunc()
{
    Vector2 mPos = find_mouse_pos();
    float r = (mPos - currLevel->player->get_pos()).length();
    // clicked too far away, do nothing
    if (r > interactRange) return;

    if (currLevel->held == nullptr)
    {
        // check to see if the player clicked on ANY entities
        int n = currLevel->gameObjects.size();
        for (int i = 0; i < n; i++)
        {
            auto obj = currLevel->gameObjects[i];
            // nothing happens when clicking on non items
            if (!obj->is_item()) continue;

            if (isInRegion(mPos, obj->get_hitbox())) {
                // right clicked an item stack, attempt to craft
                auto crafted = craftItem(obj);
                // if successful, return
                if (crafted != nullptr) return;
            }
        }
    }
    // the player didn't craft anything
    // find the cell they selected
    int sideLen = currLevel->cell_sideLen;
    Vector2Int cell(mPos.x/sideLen, mPos.y/sideLen);

    auto held = currLevel->held;

    // the type of building to be placed in the cell
    int building = (held == nullptr)? EMPTY : get_building(held->get_type());

    int status = PlaceObjectInCell(cell, building, true);
    if (status == 0 && held != nullptr) held->set_HP(held->get_hp() - 1);
}


void Game::setHeldObject( std::shared_ptr<GameObject> obj )
{
    obj->make_held();
    currLevel->held = obj;
}

void Game::throwHeldObject()
{
    // release the held object
    auto obj = currLevel->held;
    currLevel->held = nullptr;

    Vector2 mPos = find_mouse_pos();
    // move away from the player, in the direction of the mouse
    Vector2 dir = getUnitVector(currLevel->player->get_pos(), mPos);

    // copied from player position func
    float s = currLevel->player->get_moveSpeed();
    Math::Vector2 vel((bool(inputKeys&1)-bool(inputKeys&4)), (bool(inputKeys&2)-bool(inputKeys&8)));
    vel.normalise(); vel *= s;

    vel += currLevel->player->get_velocity() + (dir * 260.0f);
    Vector2 accel = vel * -0.7f;

    obj->make_thrown( vel, accel );
}



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

    bridgeTex = std::make_shared<LTexture>(window);
    if (!bridgeTex->loadFromFile("../../assets/Buildings/Bridge.png")) {
        std::cerr << "Failed to load bridge texture!" <<std::endl; 
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