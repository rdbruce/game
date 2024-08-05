#include "Game.hpp"
#include "../gameObject/AStarPathfinding.hpp"

// constructor
Game::Game( std::shared_ptr<LWindow> Window ) : window(Window)
{
    // for testing
    Bert = std::make_shared<LTexture>(window);
    if (!Bert->loadFromFile("../../assets/Bert.png")) {
        printf("Failed to load background texture!\n");
    }

    // do this first!!!
    load_textures();
    load_animations();
    load_audio();
    load_fonts();

    // camera dimensions should be the same as window size
    camera = { 0, 0, window->getWidth(), window->getHeight() };
}

float Game::get_time() { return g_time; }
bool Game::game_over() { return gameOver; }

void Game::new_game() { 
    mayGatherStone = true;
    gameOver = false;
    firstDay = true;
}

void Game::clear_input() { inputKeys = 0; }

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
                    case SDLK_LSHIFT:
                        inputKeys |= 16; break;
                    default:
                        handle_dialogue(e.key.keysym.sym); break;
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
                    case SDLK_LSHIFT:
                        inputKeys &= ~16; break;
                }
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            // left click
            switch (e.button.button)
            {
                case SDL_BUTTON_LEFT:
                    leftClickFunc(); break;
                case SDL_BUTTON_RIGHT:
                    rightClickFunc(); break;
                case SDL_BUTTON_MIDDLE:
                    if (currLevel->held != nullptr) throwHeldObject(); break;
            }
            break;
    }
}


// updates all the game objects
void Game::update_gameobjects()
{
    for (int i = 0; i < currLevel->gameObjects.size(); i++) {
        if (switching_scenes) {
            switching_scenes = false; return;
        }
        currLevel->gameObjects[i]->update();
    }
}


// finds the time elapsed between frames
void Game::update_deltaTime() {
    clock_t t = clock(); // current time
    deltaTime = float(t - begin_time) / CLOCKS_PER_SEC;

    while (deltaTime < MIN_DELTATIME)
    {
        t = clock();
        deltaTime = float(t - begin_time) / CLOCKS_PER_SEC;
    }

    begin_time = t;
}


Vector2 Game::find_mouse_pos()
{
    int x, y;
    // get the coordinates of the mouse on the screen
    SDL_GetMouseState(&x, &y);
    x /= window->getScaleX(); y /= window->getScaleY();
    x -= renderOffset.x; y -= renderOffset.y;

    // convert that to coordinates in the game
    return Vector2(x+camera.x, y+camera.y);
}


void Game::attempt_enemy_spawn()
{
    // nothing will happen if the spawning attempt is invalid
    bool validAttempt = false;

    if (isNight && g_time >= DAY_TRANSITION_TIME) {
        // spawn an enemy every 10 seconds
        if ((int(g_time)%ENEMY_SPAWN_RATE) == 0 && (int(g_time-deltaTime)%ENEMY_SPAWN_RATE) != 0) {
            validAttempt = true;
        }
    }

    if (validAttempt) 
    {
        float r = (float)rand() / RAND_MAX;
        if (r <= BIRD_SPAWN_CHANCE) spawnBird();
        else spawnWolf();
    }
}


std::shared_ptr<GameObject> Game::spawnWolf()
{
    // choose a random world edge to spawn on
    int edge = rand() % 4;
    float x, y; // coordinates the wolf will spawn at

    // set to true when a valid spawning location has been chosen
    bool validLocation = false;
    int sideLen = currLevel->cell_sideLen;

    switch (edge)
    {
        case 0: { // top edge of the map
            y = sideLen/2;
            // make x a random number between 0 and len
            x = rand()%map.w;

            // check cell below to make sure it's not water
            Vector2Int cell(x/sideLen, y/sideLen);
            if (!(currLevel->grid[cell.x][cell.y+1]&WATER)) validLocation = true;
            break;
        }
        case 1: { // left edge
            x = sideLen/2;
            y = rand() % map.h;

            // check the cell to the right to make sure it's not water
            Vector2Int cell(x/sideLen, y/sideLen);
            int n0 = (cell.y>0)? currLevel->grid[cell.x+1][cell.y-1] : 0,
                n1 = currLevel->grid[cell.x+1][cell.y],
                n2 = (cell.y<currLevel->gridDimensions.y-1)? currLevel->grid[cell.x+1][cell.y+1] : 0;
            int n = n0|n1|n2;
            if (!(n&WATER)) validLocation = true;
            break;
        }
        case 2: { // bottom edge
            y = map.h-(sideLen/2);
            x = rand() % map.w;

            // check the cell above to make sure it's not water
            Vector2Int cell(x/sideLen, y/sideLen);
            if (!(currLevel->grid[cell.x][cell.y-1]&WATER)) validLocation = true;
            break;
        }
        case 3: { // right edge
            x = map.w-(sideLen/2);
            y = rand() % map.h;

            // check the cell to the left to make sure it's not water
            Vector2Int cell(x/sideLen, y/sideLen);
            int n0 = (cell.y>0)? currLevel->grid[cell.x-1][cell.y-1] : 0,
                n1 = currLevel->grid[cell.x-1][cell.y],
                n2 = (cell.y<currLevel->gridDimensions.y-1)? currLevel->grid[cell.x-1][cell.y+1] : 0;
            int n = n0|n1|n2;
            if (!(n&WATER)) validLocation = true;
            break;
        }
    }
    // spawn a wolf at the chosen location
    if (validLocation) return Instantiate(Vector2(x,y), Wolf, -1);
    return nullptr;
}

std::shared_ptr<GameObject> Game::spawnBird()
{
    Vector2 playerPos = currLevel->player->get_pos();
    Instantiate(playerPos, Target, 1);
    birdSpawn->play();
    return Instantiate(playerPos, Bird, 1);
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
    // validation
    if (item1 == nullptr || item2 == nullptr) return nullptr;
    if (item1->is_held() || item2->is_held()) return nullptr;
    if (item1->get_timer() > 0.0f || item2->get_timer() > 0.0f) return nullptr;
    
    std::shared_ptr<GameObject> res = nullptr;
    int hp1 = item1->get_hp(), hp2 = item2->get_hp();

    switch (item1->get_type())
    {
        case Log_Item:
            switch (item2->get_type())
            {
                case Plank_Item:
                {
                    // 1 log and 4 planks craft one DAM
                    // make sure there are adequate resources
                    int num = Min(hp1, hp2/4);
                    if (num > 0)
                    {
                        res = spawnItemStack(Dam_Item, item1->get_pos(), num);
                        item1->set_HP(hp1 - num); item2->set_HP(hp2 - (4 * num));
                    }
                    break;
                }
            }
            break;

        case Plank_Item:
            switch (item2->get_type())
            {
                case Log_Item:
                {
                    // 1 logs and 4 planks craft one DAM
                    // make sure there are adequate resources
                    int num = Min(hp1/4, hp2);
                    if (num > 0)
                    {
                        res = spawnItemStack(Dam_Item, item1->get_pos(), num);
                        item1->set_HP(hp1 - (4 * num)); item2->set_HP(hp2 - num);
                    }
                    break;
                }
            }
            break;
    }
    if (res != nullptr) pop->play();
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

        case Dam_Item: {
            // DAMs may be deconstructed back into 4 planks and 1 logs
            Vector2 p = item->get_pos();
            res = spawnItemStack(Log_Item, p, 1);
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

    if (res != nullptr) pop->play();
    return res;
}


void Game::movePlayerToLevel( Scene *level, Vector2 newPlayerPos )
{
    // if the player is holding something, it should be removed from the current scene
    // and added to the new scene, as the held object
    if (currLevel->held != nullptr) {
        // revise using the same position as the player
        level->held = Instantiate(newPlayerPos, currLevel->held->get_type(), currLevel->held->get_hp(), level);
        level->held->make_held();
        Destroy(currLevel->held);
    }

    level->player = Instantiate(newPlayerPos, Player, currLevel->player->get_hp(), level);
    Destroy(currLevel->player);
    currLevel->player = nullptr;
    
    currLevel = level;
    interactRange = 3.0f * currLevel->cell_sideLen;

    // update pathfinding variables
    AStar::Open(&currLevel->grid, &barrier);

    enter_dialogue(None);

    initialise_BGTexture();

    switching_scenes = true;

    while(secondRenders.size()) secondRenders.pop();
    while(dialogueRenders.size()) dialogueRenders.pop();
}


std::shared_ptr<GameObject> Game::moveEntityToLevel( std::shared_ptr<GameObject> obj, Scene *level, Vector2 newPos )
{
    // if the object being moved is the held object, do nothing since it should move
    // with the player, not on its own
    if (obj->is_held() || obj->is_NPC()) return nullptr;


    auto res = Instantiate(newPos, obj->get_type(), obj->get_hp(), level);
    Destroy(obj);
    return res;
}


void Game::dayNightCycle()
{
    // after x seconds
    if (g_time >= DAY_LENGTH) {
        // toggle night, and make sure all scene objects get updated
        isNight = !isNight;

        // autosave the game
        save_game();
        // reset g_time to 0
        g_time = 0.0f;
        return;

    } else if (g_time == 0.0f && !isNight) {
        if (!firstDay) {
            scores.mostNightsSurvived++;
            scores.calculate_score();
        }
        else firstDay = false;
        // reset daily booleans
        mayGatherStone = true;
        // spawn npcs
        spawnNPCs();
    }

    // update time
    g_time += deltaTime;
}

void Game::enter_dialogue( Dialogue newDialogue ) { currDialogue = newDialogue; }


void Game::save_game()
{
    Base.Save("../../saves/curr/");
    Woods.Save("../../saves/curr/");
    Town.Save("../../saves/curr/");

    save_gameData("../../saves/curr/gameData.txt");
}



bool Game::is_item( int type ) { return type > ITEM_MIN && type < ITEM_MAX; }


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
        case Dam_Item: return DAM;
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
    int val = currLevel->grid[cell.x][cell.y];

    switch (val&255)
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
                for (int i = 0; i < n; i++) {
                    auto obj = currLevel->gameObjects[i];

                    if (isInRegion(mPos, obj->get_hitbox())) {
                        if (obj->is_item() && isInRegion(mPos, obj->get_hitbox()))
                        {
                            // make the player pick up the object and exit the function
                            setHeldObject(obj);
                            return;
                        } else if (obj->is_NPC()) {
                            obj->set_HP( obj->get_hp()+1 );
                            obj->set_timer( 0.25f );
                            return;
                        }
                    }
                }
                // repair whatever cell that was clicked, if possible
                int health = val&HEALTH, maxHealth = val&MAX_HEALTH;
                if (health < maxHealth) damageCell(cell, -2);

            // if an item IS held, throw it :)
            } else if (currLevel->held != nullptr) {
                if (currLevel->held->get_type() == Stone_Item && currLevel->held->get_hp() >= 2) 
                {
                    int n = currLevel->gameObjects.size();
                    for (int i = 0; i < n; i++) {
                        auto obj = currLevel->gameObjects[i];

                        if (obj->get_type() == Bear_NPC) {
                            if (isInRegion(mPos, obj->get_hitbox())) {
                                currLevel->held->set_HP(currLevel->held->get_hp()-2);
                                spawnItemStack(Berry_Item, obj->get_pos(), 1);
                                return;
                            }
                        }
                    }
                }
                throwSingleItem();
            }
            break;

    }
}


void Game::rightClickFunc()
{
    Vector2 mPos = find_mouse_pos();
    float r = (mPos - currLevel->player->get_pos()).length();
    // clicked too far away, do nothing
    if (r > interactRange) return;
    auto held = currLevel->held;

    if (held == nullptr)
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
    // if the player is holding berries, heal them
    } else if (held->get_type() == Berry_Item) {
        auto player = currLevel->player;
        int pHP = player->get_hp();
        if (pHP < player->get_maxHP()) {
            player->set_HP(pHP + 1);
            held->set_HP(held->get_hp() - 1);
        }
        return;
    } 

    // the player didn't craft anything
    // find the cell they selected
    int sideLen = currLevel->cell_sideLen;
    Vector2Int cell(mPos.x/sideLen, mPos.y/sideLen);

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
    if (inputKeys & 16) s *= 2.0f;
    Vector2 vel((bool(inputKeys&1)-bool(inputKeys&4)), (bool(inputKeys&2)-bool(inputKeys&8)));
    vel.normalise(); vel *= s;

    vel += currLevel->player->get_velocity() + (dir * (5.2f * currLevel->cell_sideLen));
    Vector2 accel = vel * -0.7f;

    obj->make_thrown( vel, accel );
}

void Game::throwSingleItem()
{
    int heldHP = currLevel->held->get_hp();
    // if there is only one item in the stack, just throw the whole thing
    if (heldHP == 1) throwHeldObject();

    else {
        // spawn a single item in the direction of the player's mouse, and give it velocity in said direction
        Vector2 mPos = find_mouse_pos();
        Vector2 pPos = currLevel->player->get_pos();
        Vector2 dir = getUnitVector(pPos, mPos);

        // copied from player position func
        float s = currLevel->player->get_moveSpeed();
        if (inputKeys&16) s *= 2.0f;
        Vector2 vel((bool(inputKeys&1)-bool(inputKeys&4)), (bool(inputKeys&2)-bool(inputKeys&8)));
        vel.normalise(); vel *= s;

        vel += currLevel->player->get_velocity() + (dir * (4.5f * currLevel->cell_sideLen));
        Vector2 accel  = vel * -0.7f;

        float r = 2.0f * (currLevel->player->get_radius() + currLevel->held->get_radius());
        Vector2 p = pPos + (dir * r);

        auto obj = Instantiate(p, currLevel->held->get_type(), 1);
        obj->make_thrown( vel, accel );

        currLevel->held->set_HP( heldHP - 1 );
    }
}

void Game::spawnNPCs() 
{
    // spawn NPCs in the town level
    int n = Town.gameObjects.size(), i;
    for (i = 0; i < n; i++) {
        if (Town.gameObjects[i]->get_type() == Fox_NPC) break;
    }
    if (i == n) {
        Vector2 pos(913.0f, -50.0f);
        Instantiate(pos, Fox_NPC, 1, &Town);
    }

    // spawn NPCS in the base level
    if (scores.mostNightsSurvived == 0) 
    {
        n = Base.gameObjects.size();
        for (i = 0; i < n; i++) {
            if (Base.gameObjects[i]->get_type() == Fox_NPC) break;
        }
        if (i == n) {
            Vector2 pos(2000.0f, 3050.0f);
            Instantiate(pos, Fox_NPC, 1, &Base);
        }
    }
}


void Game::load_levels( std::string dir )
{
    Base = Scene(dir + "Base.txt", this);
    if (Base.player != nullptr) currLevel = &Base;

    Woods = Scene(dir + "Woods.txt", this);
    if (Woods.player != nullptr) currLevel = &Woods;

    Town = Scene(dir + "Town.txt", this);
    if (Town.player != nullptr) currLevel = &Town;

    // set up pointers to adjacent levels
    Base.assignNeighbours(&Town, &Woods);
    Woods.assignNeighbours(&Base, nullptr);
    Town.assignNeighbours(nullptr, &Base);

    isNight = false;
    g_time = 0.0f;

    interactRange = 3.0f * currLevel->cell_sideLen;

    load_gameData(dir + "gameData.txt");
}

void Game::load_gameData( std::string filename )
{
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Couldn't open " << filename << std::endl;
    } else {
        std::string line;
        std::getline(file, line);

        std::istringstream iss( line );

        iss >> std::dec >> isNight >> mayGatherStone;
        file.close();
    }
}

void Game::save_gameData( std::string filename )
{
    std::fstream file;
    file.open(filename, std::ios::out);
    if (!file) {
        std::cerr << "Failed to open " << filename <<std::endl;
    } else {
        file << std::dec << isNight <<'\t'<< mayGatherStone;
        file.close();
    }
}

bool Game::is_under_tree( Vector2Int cell )
{
    int minY = Max(cell.y, 0), maxY = Min(currLevel->gridDimensions.y-1, cell.y+9),
        minX = Max(cell.x-1, 0), maxX = Min(cell.x+1, currLevel->gridDimensions.x-1);


    for (int x = minX; x <= maxX; x++) 
    {
        for (int y = minY; y <= maxY; y++) 
        {
            if ((currLevel->grid[x][y]&CELL_ID) == 3) return true;
        }
    }
    return false;
}


void Game::load_textures()
{
    fpsTex = std::make_unique<LTexture>(window);
    controlsTex = std::make_unique<LTexture>(window);
    // load all the textures from their image files

    grassTex = std::make_shared<LTexture>(window);
    if (!grassTex->loadFromFile("../../assets/Ground/Grass.png")) {
        std::cerr << "Failed to load grass texture!" << std::endl;
    }

    logTex = std::make_shared<LTexture>(window);
    if (!logTex->loadFromFile("../../assets/Buildings/Log.png")) {
        std::cerr << "Failed to load log texture!" << std::endl;
    }

    damTex = std::make_shared<LTexture>(window);
    if (!damTex->loadFromFile("../../assets/Buildings/Dam.png")) {
        std::cerr << "Failed to load DAM texture!" <<std::endl; 
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
    playerTex = std::make_shared<LTexture>(window);
    if (!playerTex->loadFromFile("../../assets/Entities/Player/front/0.png")) {
        std::cerr << "Failed to load texture for player!" << std::endl;
    }
    wolfTex = std::make_shared<LTexture>(window);
    if (!wolfTex->loadFromFile("../../assets/Entities/Wolf.png")) {
        std::cerr << "Failed to load texture for wolf!" << std::endl;
    }
    falling_treeTex = std::make_shared<LTexture>(window);
    if (!falling_treeTex->loadFromFile("../../assets/Tree/Falling_Tree.png")) {
        std::cerr << "Failed to load texture for falling tree!" << std::endl;
    }
    pine_coneTex = std::make_shared<LTexture>(window);
    if (!pine_coneTex->loadFromFile("../../assets/Items/Pine_Cone.png")) {
        std::cerr << "Failed to load texture for pine cone!" << std::endl;
    }
    plankTex = std::make_shared<LTexture>(window);
    if (!plankTex->loadFromFile("../../assets/Items/Plank.png")) {
        std::cerr << "Failed to load texture for plank!" << std::endl;
    }

    foxTex = std::make_shared<LTexture>(window);
    if (!foxTex->loadFromFile("../../assets/Entities/Fox.png")) {
        std::cerr << "Failed to load texture for fox!" << std::endl;
    }
    berryTex = std::make_shared<LTexture>(window);
    if (!berryTex->loadFromFile("../../assets/Items/Berry.png")) {
        std::cerr << "Failed to load texture for berry!" << std::endl;
    }
    dirtTex = std::make_shared<LTexture>(window);
    if (!dirtTex->loadFromFile("../../assets/Ground/Dirt.png")) {
        std::cerr << "Failed to load texture for dirt!" << std::endl;
    }
    berry_bushTex = std::make_shared<LTexture>(window);
    if (!berry_bushTex->loadFromFile("../../assets/Buildings/Berry_Bush.png")) {
        std::cerr << "Failed to load texture for berry bush!" << std::endl;
    }
    empty_bushTex = std::make_shared<LTexture>(window);
    if (!empty_bushTex->loadFromFile("../../assets/Buildings/EmptyBush.png")) {
        std::cerr << "Failed to load texture for depleted bush!" << std::endl;
    }
    full_heartTex = std::make_shared<LTexture>(window);
    if (!full_heartTex->loadFromFile("../../assets/Menu/FullHeart.png")) {
        std::cerr << "Failed to load texture for full heart!" << std::endl;
    }
    empty_heartTex = std::make_shared<LTexture>(window);
    if (!empty_heartTex->loadFromFile("../../assets/Menu/EmptyHeart.png")) {
        std::cerr << "Failed to load texture for empty heart!" << std::endl;
    }
    stoneTex = std::make_shared<LTexture>(window);
    if (!stoneTex->loadFromFile("../../assets/Items/Stone.png")) {
        std::cerr << "Failed  to load texture for stone!" << std::endl;
    }
    LMBTex = std::make_shared<LTexture>(window);
    if (!LMBTex->loadFromFile("../../assets/Menu/Mouse/LeftClick.png")) {
        std::cerr << "failed to load left mouse texture!" << std::endl;
    }
    MMBTex = std::make_shared<LTexture>(window);
    if (!MMBTex->loadFromFile("../../assets/Menu/Mouse/MiddleClick.png")) {
        std::cerr << "failed to load middle mouse texture!" << std::endl;
    }
    RMBTex = std::make_shared<LTexture>(window);
    if (!RMBTex->loadFromFile("../../assets/Menu/Mouse/RightClick.png")) {
        std::cerr << "failed to load right mouse texture!" << std::endl;
    }
    BearTex = std::make_shared<LTexture>(window);
    if (!BearTex->loadFromFile("../../assets/Entities/Bear.png")) {
        std::cerr << "Failed to load bear texture!" << std::endl;
    }
    BirdTex = std::make_shared<LTexture>(window);
    if (!BirdTex->loadFromFile("../../assets/Entities/Bird.png")) {
        std::cerr << "Failed to load texture for bird!" << std::endl;
    }
    BombTex = std::make_shared<LTexture>(window);
    if (!BombTex->loadFromFile("../../assets/Entities/Bomb.png")) {
        std::cerr << "Failed to load texture for bomb!" << std::endl;
    }
    TargetTex = std::make_shared<LTexture>(window);
    if (!TargetTex->loadFromFile("../../assets/Target.png")) {
        std::cerr << "Failed to load texture for target!" << std::endl;
    }
    dashed_circleTex = std::make_shared<LTexture>(window);
    if (!dashed_circleTex->loadFromFile("../../assets/DashedCircle.png")) {
        std::cerr << "Failed to load texture for explosion indicator!" << std::endl;
    }
}

void Game::load_audio()
{
    logDestruction = std::make_shared<LAudio>();
    if (!logDestruction->loadFromFile("../../assets/Audio/Destruction/LogBreaking.wav")) {
        std::cerr << "Failed to load audio for log breaking!" << std::endl;
    }

    treeFalling = std::make_shared<LAudio>();
    if (!treeFalling->loadFromFile("../../assets/Audio/Destruction/TreeFalling.wav")) {
        std::cerr << "Failed to load audio for tree falling!" << std::endl;
    }

    doorToggle = std::make_shared<LAudio>();
    if (!doorToggle->loadFromFile("../../assets/Audio/DoorToggle.wav")) {
        std::cerr << "Failed to load audio for door opening!" << std::endl;
    }
    
    leaves = std::make_shared<LAudio>();
    if (!leaves->loadFromFile("../../assets/Audio/Destruction/Leaves.wav")) {
        std::cerr << "Failed to load audio for leaves!" << std::endl;
    }

    pop = std::make_shared<LAudio>();
    if (!pop->loadFromFile("../../assets/Audio/EntitySounds/Pop.wav")) {
        std::cerr << "Failed to load item pop sound!" << std::endl;
    }

    bonk = std::make_shared<LAudio>();
    if (!bonk->loadFromFile("../../assets/Audio/EntitySounds/Bonk.wav")) {
        std::cerr << "Failed to load bonk sound!" << std::endl;
    }
    birdSpawn = std::make_shared<LAudio>();
    if (!birdSpawn->loadFromFile("../../assets/Audio/EntitySounds/BirdSpawn.wav")) {
        std::cerr << "Failed to load bird spawn sound!" << std::endl;
    }
}

void Game::load_fonts()
{
    sevenSegment = TTF_OpenFont("../../assets/Fonts/Seven_Segment.ttf", 48);
    if (sevenSegment == NULL) {
        std::cerr << "Failed to load seven segment font!" << std::endl;
    }
}

void Game::load_animations()
{
    // player animations
    std::vector<std::shared_ptr<LTexture>> front, back, left, right;
    float df = 1.0f, db = 1.0f, dl = 1.0f, dr = 1.0f;

    std::string path = "../../assets/Entities/Player/";
    std::string path1 = path + "back/";
    for (int i = 0; i < 4; i++) 
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path1 + std::to_string(i) + ".png";
        if (!newTex->loadFromFile(filename)) break;
        back.push_back(newTex);
    }

    path1 = path + "front/";    
    for (int i = 0; i < 4; i++) 
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path1 + std::to_string(i) + ".png";
        if (!newTex->loadFromFile(filename)) break;
        front.push_back(newTex);
    }
    
    path1 = path + "left/";    
    for (int i = 0; i < 4; i++) 
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path1 + std::to_string(i) + ".png";
        if (!newTex->loadFromFile(filename)) break;
        left.push_back(newTex);
    }
    
    path1 = path + "right/";    
    for (int i = 0; i < 4; i++) 
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path1 + std::to_string(i) + ".png";
        if (!newTex->loadFromFile(filename)) break;
        right.push_back(newTex);
    }

    playerAnimations = std::make_shared<Animations>(front, back, left, right, df, db, dl, dr);
}