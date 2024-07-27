#include "GameObject.hpp"
#include "../game/Game.hpp"
#include "AStarPathfinding.hpp"


void GameObject::wolfVelocityFunc()
{
    damage = 0;

    // wolves leave the map when it becomes day
    if (!game->isNight) beginRetreat();
    else {
        // wolves will update their pathfinding every 0.5 seconds
        if (timer <= 0.0f || path == nullptr) {
            wolfUpdatePathfinding();
        }

        // if the wolf is close enough to the player, attempt to perform an attack
        if (pathfindingLength < 5) 
        {
            Vector2 distToPlayer = get_player_pos() - pos;
            int sideLen = game->currLevel->cell_sideLen;

            // set the pathfinding to a nullptr, so it's not stuck in this case forever
            // if the wolf is super close, just go straight for them
            if (distToPlayer.length() <= 2.0f * sideLen || pathfindingLength < 2) {
                path = nullptr;
                damage = 1;
                moveDirectlyToPlayer();

            } else if (timer <= 0.5f) {
                path = nullptr;
                wolfEnterJumpAttack();
            }
        }
        // decrease the timer
        timer -= get_deltaTime();

        // walk to the first cell in its pathfinding
        if (path != nullptr) wolfWalkToNextCell();
    }
}


void GameObject::wolfJumpAttack()
{
    // spend 0.5 seconds charging up
    if (acceleration == Vector2_Zero && timer <= 2.0f && timer >= 1.9f)
    {
        Vector2 pPos = get_player_pos();
        Vector2 dir = getUnitVector(pos, pPos);
        // give the wolf a bunch of velocity in that direction,
        // with some negative acceleration
        velocity = dir * (7.0f * game->currLevel->cell_sideLen);
        acceleration = dir * -300.0f;

        // wolf deals increased damage during this attack
        damage =2;

    // when the wolf comes to a stop, go back to the regular pathfinding
    } else if (velocity * acceleration > 0.0f) {
        // reset any velocity + acceleration
        acceleration = velocity = Vector2_Zero;
    
    // spend a second cooling down
    } else if (timer <= 0.0f) {
        // reset the timer
        timer = 1.0f;
        damage = 0;
        // set the velocity back to regular pathfinding
        wolfUpdatePathfinding();
        velocityFunc = &GameObject::wolfVelocityFunc;
        return;
    }
    // add acceleration to velocity
    float dt = get_deltaTime();
    velocity += acceleration * dt;
    // decrease the timer
    timer -= dt;
}


void GameObject::wolfSpawningFunc()
{
    Vector2Int cell = get_cell();
    if (game->is_barrier(cell)) {
        // still inside the world border, move towards the centre of the map
        Vector2Int dest = get_mapDimensions()/2;
        Vector2 dir = getUnitVector(pos, Vector2(dest.x, dest.y));
        // don't use the entity's speed, just go with a constant speed while spawning
        velocity = dir * 80.0f;

    } else {
        // entity is now in the map, transition into the normal pathfinding algorithm
        hasCollision = true;
        velocityFunc = &GameObject::wolfVelocityFunc;
    }
}

void GameObject::foxVelocityFunc()
{
    if (game->isNight) beginRetreat();
    else {
        if (!hasCollision) {
            Vector2Int cell = get_cell();
            if (!game->is_barrier(cell)) {
                hasCollision = true;
            }
        }
        deccelerateVelocityFunc();
    }
}


void GameObject::defaultVelocityFunc() {
    velocity += acceleration * get_deltaTime();
}

void GameObject::deccelerateVelocityFunc() {
    if (acceleration * velocity > 0.0f) {
        acceleration = velocity = Vector2_Zero;
    }
    velocity += acceleration * get_deltaTime();
}



void GameObject::moveDirectlyToPlayer()
{
    Vector2 dir = getUnitVector( pos, get_player_pos() );
    if (velocity * acceleration > 0.0f) acceleration = Vector2_Zero;
    velocity = (dir * moveSpeed) + (acceleration * get_deltaTime());
}

void GameObject::retreatVelocityFunc()
{
    // move away from the centre of the map until out of bounds
    Vector2Int map = get_mapDimensions();
    Vector2 mCentre(map.x/2, map.y/2);

    Vector2 dir = getUnitVector(mCentre, pos);
    // in case the entity is on the map's centre
    if (dir == Vector2_Zero) dir = Vector2_One;

    // accelerate off the map
    acceleration = dir * 1500.0f;
    velocity += acceleration * get_deltaTime();

    // destroy itself when it goes out of bounds
    if (pos.x != clampf(0.0f, map.x, pos.x) || pos.y != clampf(0.0f, map.y, pos.y)) {
        game->Destroy(game->currLevel->gameObjects[idx]);
    }
}







void GameObject::playerPositionFunc()
{
    float s = moveSpeed;
    Uint8 inputs = get_inputKeys();
    float dt = get_deltaTime();
    Vector2Int map = get_mapDimensions();

    if (inputs&16) s *= 2.0f;

    Vector2 vel(
        // horizontal velocity: bit 1=d; move right (+x), bit 3=a; move left (-x)
        (bool(inputs&1)-bool(inputs&4)),
        // vertical velocity: bit 2=s; move down (+y), bit 4=w; move up (-y)
        (bool(inputs&2)-bool(inputs&8))
    );

    // normalise the vector so that players can't walk faster by moving diagonally
    vel.normalise(); 
    vel *= s; // multiply the unit vector by movement speed

    Vector2 p = pos + (velocity+vel)*dt;

    set_pos(p);
}


void GameObject::foxPositionFunc()
{
    // attempt to move back to a point
    Vector2 target = (game->currLevel == &game->Base)? Vector2(1500.0f, 1950.0f) : Vector2(609.0f, 230.0f);
    Vector2 dist = target - pos;
    Vector2 vel = Vector2_Zero;

    if (dist.length() > 1.0f * game->currLevel->cell_sideLen) {
        dist.normalise();
        vel = dist * moveSpeed;
    }
    float dt = get_deltaTime();

    Vector2 p = pos + (velocity+vel) * dt;
    set_pos(p);
}


void GameObject::defaultPositionFunc()
{
    Vector2 newPos = pos + velocity * get_deltaTime();
    set_pos(newPos);
}

void GameObject::bearPositionFunc()
{
    Vector2 newPos(1240.0f, 200.0f);
    set_pos(newPos);
}

void GameObject::fallingTreePositionFunc()
{
    // when the timer reaches 0, destroy the object
    if (timer <= 0.0f)
    {
        // once the tree is gone, spawn some log + pine cone items in where it fell
        // the further log will be the height of the tree (size.x) to the LEFT of pos,
        // the nearest one could be at pos

        // 5 wood will spawn, 0-2 seeds will spawn
        int seeds = rand()%3, n = seeds+5;

        Vector2Int size = get_size();
        int sideLen = game->currLevel->cell_sideLen;
        
        float step = (sideLen*10)/n, y = hitbox.y + hitbox.h;

        // spawn n items
        for (int i = 1; i <= n; i++)
        {
            Vector2 p(hitbox.x - step*i, y);
            // spawn logs first, seeds last
            EntityType item = (i<6)? Log_Item : Pine_Cone_Item;
            game->spawnItemStack(item, p, 1);
        }
        // revise this line when you get the chance vvv
        game->Destroy(game->currLevel->gameObjects[idx]);
    }
}


void GameObject::heldItemPositionFunc()
{
    // get a unit vector from the player's centre to the mouse position's centre
    Vector2 mPos = game->find_mouse_pos(), pPos = get_player_pos();
    Vector2 dir = getUnitVector( pPos, mPos );

    // place the item a fixes distance from the player, in the direction of the mouse
    float r = game->currLevel->player->radius + radius + 5.0f;
    set_pos( pPos + (dir * r) );
}


void GameObject::thrownItemPositionFunc()
{
    float dt = get_deltaTime();
    Vector2 newPos = pos + (velocity * dt);
    set_pos(newPos);

    // update the timer
    timer -= dt;
}



void GameObject::wolfUpdatePathfinding()
{
    if (path != nullptr) {
        // save the current pathfinding cell
        Vector2Int currCell = path->cell;

        // clean up all existing pathfinding nodes
        AStar::LinkedCell *curr = path;
        for (AStar::LinkedCell *next = curr; curr != nullptr; curr = next) {
            next = curr->next;
            delete curr;
        }

        // update pathfinding
        pathfindToPlayer(50);

        // if the new pathfinding cell is different than the new cell, then the wolf was in
        // the process of moving to the next node, progress the node to represent this.
        // sometimes, two wolves can get each other stuck by trying to move into the same cell
        // since they push each other back and never reach the centre, they are stuck forever.
        // to break this "stalemate", give the wolf a 50% chance to progress to the next node
        float random = float(rand()) / RAND_MAX;
        if (path->cell != currCell || random < 0.5f) {
            path = path->next;
        }
        // reset the timer
        timer = 0.5f;
    
    } else pathfindToPlayer(50);
}


void GameObject::wolfWalkToNextCell()
{
    // try to get to the centre of the first pathfinding cell
    int sideLen = game->currLevel->cell_sideLen;
    Vector2Int cell = path->cell;
    Vector2 cellPos(((float)cell.x+0.5f)*sideLen, ((float)cell.y+0.5f)*sideLen);

    // get the displacement form the wolf to the centre of the next cell
    Vector2 disp = cellPos - pos;
    Vector2 dir = Vector2_Zero;
    float len = disp.length();

    // if it is close enough (within one unit), progress to the next node
    // otherwise, move towards the cell centre
    if (len <= 2.0f) {
        // make the position exact
        set_pos( cellPos );
        // progress to the next node
        path = path->next;
    } else {
        // move towards the cell's centre
        dir = disp / len;
    }
    if (velocity * acceleration > 0.0f) acceleration = Vector2_Zero;
    velocity = (dir * moveSpeed) + (acceleration * get_deltaTime());
}




void GameObject::pathfindToPlayer( int searchDepth )
{
    AStar::Open(&game->currLevel->grid, &game->barrier);

    // create a new pathfinding object
    Vector2Int  start = get_cell(), 
                end = game->currLevel->player->get_cell(),
                dimensions(game->currLevel->gridDimensions.x, game->currLevel->gridDimensions.y);

    AStar::Pathfinding pathfinding(start, end, dimensions, searchDepth);

    // find the best path to the player
    AStar::LinkedCell *node = pathfinding.findPath();
    // reverse the list to be start to end
    path = AStar::ReverseLinkedList(node);
    pathfinding.close();

    // count the number of nodes in the linked list
    int count = 0;
    for (AStar::LinkedCell *curr = path; curr != nullptr; curr = curr->next) {
        count++;
    }
    pathfindingLength = count;
}



void GameObject::wolfEnterJumpAttack()
{
    // attack will last 2.75 seconds
    timer = 2.75f;
    // give the wolf acceleration so that it will come to a stop in 0.5 seconds
    acceleration = velocity * -2.0f;
    // switch to the attack function
    velocityFunc = &GameObject::wolfJumpAttack;
}

void GameObject::beginRetreat()
{
    // turn off collision and switch to retreat function
    hasCollision = false;
    // start with some velocity towards the centre of the map
    Vector2Int map = get_mapDimensions();
    Vector2 mCentre(map.x/2, map.y/2);
    velocity = getUnitVector(pos, mCentre) * 300.0f;
    // enter the retreat function
    velocityFunc = &GameObject::retreatVelocityFunc;
    positionFunc = &GameObject::defaultPositionFunc;
}


void GameObject::defaultRenderFunc( int camX, int camY, Uint8 alpha )
{
    Vector2Int p( hitbox.x-camX, hitbox.y-camY );
    // not within the camera's view, don't render
    if (p.x != Clamp(-hitbox.w, game->camera.w, p.x) || p.y != Clamp(-hitbox.h, game->camera.h, p.y)) {
        return;
    }
    tex->setAlpha(alpha);
    tex->render(p.x, p.y, &hitbox);
    tex->setAlpha(255);


    Vector2Int cell = get_cell();
    if (game->is_under_tree(cell) && alpha == 255 && alpha == 255) {
        game->secondRenders.push(this);
    }
    // show hp
    else if (hp < max_hp && hp > 0) {
        float t = (float)hp / (float)max_hp;
        int wWhite = hitbox.w*t, wRed = hitbox.w * (1.0f-t);

        auto white = game->tEditor.createSolidColour(wWhite, 20, 0xFFFFFFFF, game->window);
        auto red   = game->tEditor.createSolidColour(wRed, 20, 0xFF0000FF, game->window);

        SDL_Rect wRect = {p.x, p.y, wWhite, 20}, rRect = {p.x+wWhite, p.y, wRed, 20};

        white->render(wRect.x, wRect.y, &wRect);
        red->render(rRect.x, rRect.y, &rRect);
    }
}

void GameObject::fallingTreeRenderFunc( int camX, int camY, Uint8 alpha )
{
    Vector2Int p( hitbox.x - camX, hitbox.y-camY );
    // not within the camera's view, don't render
    if (p.x != Clamp(-hitbox.w, game->camera.w, p.x) || p.y != Clamp(-hitbox.h, game->camera.h, p.y)) {
        return;
    }
    // timer is initialised to 1.0f, use it as an interpolator
    float interp = 1.0f - timer;
    float theta = -100.0f * interp;
    SDL_Point centre = { hitbox.w/2, hitbox.h };
    tex->render(p.x, p.y, &hitbox, NULL, theta, &centre);
    timer -= get_deltaTime();
}

void GameObject::itemRenderFunc( int camX, int camY, Uint8 alpha )
{
    Vector2Int p( hitbox.x - camX, hitbox.y-camY );
    // not within the camera's view, don't render
    if (p.x != Clamp(-hitbox.w, game->camera.w, p.x) || p.y != Clamp(-hitbox.h, game->camera.h, p.y)) {
        return;
    }
    tex->setAlpha(alpha);
    tex->render(p.x, p.y, &hitbox);
    tex->setAlpha(255);


    Vector2Int cell = get_cell();
    if (game->is_under_tree(cell) && alpha == 255) {
        game->secondRenders.push(this);
    
    } else {
        std::string txt = std::to_string(hp);
        auto itemCountTex = std::make_unique<LTexture>(game->window);
        if (!itemCountTex->loadFromRenderedText(txt, {255,255,255,255})) {
            std::cerr << "failed to load health display!" << std::endl;
        }
        itemCountTex->render(p.x+hitbox.w-(itemCountTex->getWidth()/2), p.y+hitbox.h-(itemCountTex->getHeight()/2));
    }
}