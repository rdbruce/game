#include "GameObject.hpp"
#include "../game/Game.hpp"

// constructor
GameObject::GameObject( Vector2 pos, EntityType Type, int Idx, int Health, Game *game, int sideLen ) 
: pos(pos), game(game), type(Type), idx(Idx)
{
    switch (type)
    {
        case Player: {
            // assign the texture
            tex = game->playerTex;
            altTex = std::make_shared<LTexture>(game->window);
            renderingFunc = &GameObject::playerRenderFunc;

            // set up the hitbox
            Vector2Int size(sideLen-2, sideLen-2);
            Vector2Int p = Vector2Int(pos.x, pos.y) - (size/2);
            hitbox = { p.x, p.y, size.x, size.y };
            radius = Max(size.x/2, size.y/2);

            // assign behaviour functions
            velocityFunc = &GameObject::deccelerateVelocityFunc;
            positionFunc = &GameObject::playerPositionFunc;
            collisionFunc = &GameObject::playerCollisionFunction;

            // other attributes
            moveSpeed = 2.66666667f * sideLen;
            max_hp = 5;
            hp = (Health == -1)? max_hp : Clamp(0, max_hp, Health);
            hasCollision = true;
            break;
        }

        
        case Wolf: {
            // assign the texture
            tex = game->wolfTex;
            renderingFunc = &GameObject::defaultRenderFunc;

            // set up the hitbox
            Vector2Int size(sideLen-2, sideLen-2);
            Vector2Int p = Vector2Int(pos.x, pos.y) - (size/2);
            hitbox = { p.x, p.y, size.x, size.y };
            radius = Max(size.x/2, size.y/2);

            // assign behaviour functions
            velocityFunc = &GameObject::wolfSpawningFunc;
            positionFunc = &GameObject::defaultPositionFunc;
            collisionFunc = &GameObject::defaultCollisionFunction;

            // other attributes
            moveSpeed = 3.2f * sideLen;
            max_hp = 3;
            hp = (Health == -1)? max_hp : Clamp(0, max_hp, Health);
            hasCollision = false;
            break;
        }


        case Falling_Tree: {
            // assign the texture
            tex = game->falling_treeTex;
            renderingFunc = &GameObject::fallingTreeRenderFunc;

            // set up the hitbox
            Vector2Int size(3*sideLen, 10*sideLen);
            Vector2Int p = Vector2Int(pos.x, pos.y) - (size/2);
            hitbox = { p.x, p.y, size.x, size.y };

            // assign behaviour functions
            velocityFunc = &GameObject::defaultVelocityFunc;
            positionFunc = &GameObject::fallingTreePositionFunc;
            collisionFunc = &GameObject::defaultCollisionFunction;

            // other attributes
            max_hp = hp = 1;
            timer = 1.0f;
            hasCollision = false;
            break;
        }

        case Log_Item: {
            // assign the texture
            tex = game->logTex;
            renderingFunc = &GameObject::defaultRenderFunc;

            // set up the hitbox
            Vector2Int size(sideLen-20, sideLen-20);
            Vector2Int p = Vector2Int(pos.x, pos.y) - (size/2);
            hitbox = { p.x, p.y, size.x, size.y };
            radius = Max(size.x/2, size.y/2);

            // assign behaviour functions
            velocityFunc = &GameObject::deccelerateVelocityFunc;
            positionFunc = &GameObject::thrownItemPositionFunc;
            collisionFunc = &GameObject::stationaryItemCollisionFunction;

            // other attributes
            max_hp = 5;
            hp = (Health == -1)? max_hp : Clamp(0, max_hp, Health);
            hasCollision = true;
            moveSpeed = 0.5f;
            break;
        }
        
        case Pine_Cone_Item: {
            // assign the texture
            tex = game->pine_coneTex;
            renderingFunc = &GameObject::defaultRenderFunc;

            // set up the hitbox
            Vector2Int size(sideLen-20, sideLen-20);
            Vector2Int p = Vector2Int(pos.x, pos.y) - (size/2);
            hitbox = { p.x, p.y, size.x, size.y };
            radius = Max(size.x/2, size.y/2);

            // assign behaviour functions
            velocityFunc = &GameObject::deccelerateVelocityFunc;
            positionFunc = &GameObject::thrownItemPositionFunc;
            collisionFunc = &GameObject::stationaryItemCollisionFunction;

            // other attributes
            max_hp = 6;
            hp = (Health == -1)? max_hp : Clamp(0, max_hp, Health);
            hasCollision = true;
            moveSpeed = 0.333333333f;
            break;
        }

        case Plank_Item: {
            // assign the texture
            tex = game->plankTex;
            renderingFunc = &GameObject::defaultRenderFunc;

            // set up the hitbox
            Vector2Int size(sideLen-20, sideLen-20);
            Vector2Int p = Vector2Int(pos.x, pos.y) - (size/2);
            hitbox = { p.x, p.y, size.x, size.y };
            radius = Max(size.x/2, size.y/2);

            // assign behaviour functions
            velocityFunc = &GameObject::deccelerateVelocityFunc;
            positionFunc = &GameObject::thrownItemPositionFunc;
            collisionFunc = &GameObject::stationaryItemCollisionFunction;

            // other attributes
            max_hp = 10;
            hp = (Health == -1)? max_hp : Clamp(0, max_hp, Health);
            hasCollision = true;
            moveSpeed = 0.5f;
            break;
        }
        
        case Bridge_Item: {
            // assign the texture
            tex = game->bridgeTex;
            renderingFunc = &GameObject::defaultRenderFunc;

            // set up the hitbox
            Vector2Int size(sideLen-20, sideLen-20);
            Vector2Int p = Vector2Int(pos.x, pos.y) - (size/2);
            hitbox = { p.x, p.y, size.x, size.y };
            radius = Max(size.x/2, size.y/2);

            // assign behaviour functions
            velocityFunc = &GameObject::deccelerateVelocityFunc;
            positionFunc = &GameObject::thrownItemPositionFunc;
            collisionFunc = &GameObject::stationaryItemCollisionFunction;

            // other attributes
            max_hp = 8;
            hp = (Health == -1)? max_hp : Clamp(0, max_hp, Health);
            hasCollision = true;
            moveSpeed = 0.5f;
            break;
        }
        
        
        case Door_Item: {
            // assign the texture
            tex = game->closed_doorTex;
            renderingFunc = &GameObject::defaultRenderFunc;

            // set up the hitbox
            Vector2Int size(sideLen-20, sideLen-20);
            Vector2Int p = Vector2Int(pos.x, pos.y) - (size/2);
            hitbox = { p.x, p.y, size.x, size.y };
            radius = Max(size.x/2, size.y/2);

            // assign behaviour functions
            velocityFunc = &GameObject::deccelerateVelocityFunc;
            positionFunc = &GameObject::thrownItemPositionFunc;
            collisionFunc = &GameObject::stationaryItemCollisionFunction;

            // other attributes
            max_hp = 4;
            hp = (Health == -1)? max_hp : Clamp(0, max_hp, Health);
            hasCollision = true;
            moveSpeed = 0.5f;
            break;
        }

        case Fox_NPC:
            // assign the texture
            tex = game->foxTex;
            altTex = std::make_shared<LTexture>(game->window);
            renderingFunc = &GameObject::foxRenderFunc;

            // set up the hitbox
            Vector2Int size(sideLen-2, sideLen-2);
            Vector2Int p = Vector2Int(pos.x, pos.y) - (size/2);
            hitbox = { p.x, p.y, size.x, size.y };
            radius = Max(size.x/2, size.y/2);

            // assign behaviour functions
            positionFunc = &GameObject::defaultPositionFunc;
            velocityFunc = &GameObject::deccelerateVelocityFunc;
            collisionFunc = &GameObject::defaultCollisionFunction;

            // other attributes
            max_hp = 255;
            hp = Health;
            hasCollision = true;
            break;
    }
}
GameObject::GameObject() : type(Log_Item), idx(-1) {}

void GameObject::render( int camX, int camY )
{
    (this->*renderingFunc)( camX, camY );
}

void GameObject::update()
{
    (this->*positionFunc)();
    (this->*velocityFunc)();
    (this->*collisionFunc)();

    // destroy itself if hp reaches 0
    if (hp <= 0) {
        // destroying the player causes issues, determine something else to do when they die
        if (type != Player)
        game->Destroy(game->currLevel->gameObjects[idx]);
    
    } else {
        hp = Clamp(0, max_hp, hp);
    }
}


void GameObject::make_held()
{
    velocity = acceleration = Vector2_Zero;
    timer = 0.0f;
    positionFunc = &GameObject::heldItemPositionFunc;
}

void GameObject::make_thrown( Vector2 newVelocity, Vector2 newAcceleration )
{
    velocity = newVelocity;
    acceleration = newAcceleration;
    positionFunc = &GameObject::thrownItemPositionFunc;
}


bool GameObject::is_item() { return type>=Log_Item && type<=Door_Item; }
bool GameObject::is_enemy() { return type>=Wolf && type<=Wolf; }


// mutators
void GameObject::set_pos( Vector2 newPos ) { 
    pos = newPos; 
    hitbox.x = pos.x - (hitbox.w/2);
    hitbox.y = pos.y - (hitbox.h/2);
}
void GameObject::set_velocity ( Vector2 newVelocity ) { velocity = newVelocity; }
void GameObject::set_acceleration( Vector2 newAcceleration ) { acceleration = newAcceleration; }

void GameObject::set_HP( int newHP ) { hp = Clamp(0, max_hp, newHP); }
void GameObject::set_timer( float t ) { timer = t; }
void GameObject::decrement_idx() { --idx; }


// accessors
Vector2 GameObject::get_pos() { return pos; }
Vector2 GameObject::get_velocity() { return velocity; }
Vector2 GameObject::get_acceleration() { return acceleration; }
int GameObject::get_idx() { return idx; }
EntityType GameObject::get_type() { return type; }
int GameObject::get_hp() { return hp; }
int GameObject::get_maxHP() {return max_hp; }
int GameObject::get_damage() { return damage; }
float GameObject::get_moveSpeed() { return moveSpeed; }
float GameObject::get_radius() { return radius; }
float GameObject::get_timer() { return timer; }
bool GameObject::has_collision() { return hasCollision; }
bool GameObject::is_held() { return game->currLevel->gameObjects[idx] == game->currLevel->held; }
Uint8 GameObject::get_inputKeys() { return game->inputKeys; }
float GameObject::get_deltaTime() { return game->deltaTime; }
SDL_Rect GameObject::get_hitbox() { return hitbox; }
Vector2Int GameObject::get_size() { return Vector2Int(hitbox.w, hitbox.h); }
Vector2Int GameObject::get_mapDimensions() { return Vector2Int(game->map.w, game->map.h); }
Vector2Int GameObject::get_cell() {
    int sideLen = game->currLevel->cell_sideLen;
    return Vector2Int( pos.x/sideLen, pos.y/sideLen );
}
Vector2 GameObject::get_player_pos() { return game->currLevel->player->pos; };