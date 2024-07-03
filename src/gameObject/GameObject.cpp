#include "GameObject.hpp"
#include "../game/Game.hpp"

// constructor
GameObject::GameObject( Vector2 pos, EntityType Type, int Idx, int Health, Game *game, int sideLen ) 
: pos(pos), game(game), type(Type), idx(Idx)
{
    switch (type)
    {
        case Player:
            // assign the texture
            tex = LTexture( game->window );
            if (!tex.loadFromFile("../../assets/Entities/Player/front/0.png")) {
                printf("Failed to load entity texture!\n");
            }

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
            moveSpeed = 200.0f;
            max_hp = 5;
            hp = Clamp(0, max_hp, Health);
            hasCollision = true;
            break;
    }
}
GameObject::GameObject() : type(Log_Item), idx(-1) {}

GameObject::~GameObject()
{
    // destroy the texture
    tex.~LTexture();
}


void GameObject::render( int camX, int camY )
{
    Vector2Int p( hitbox.x-camX, hitbox.y-camY );
    tex.render( p.x, p.y, &hitbox );
}


void GameObject::update()
{
    (this->*positionFunc)();
    (this->*velocityFunc)();
    (this->*collisionFunc)();
}


bool GameObject::is_item() { return type>=Log_Item; }
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

void GameObject::change_behaviour_function( BehaviourFunc func, void (GameObject::*newFunc)(void) )
{
    switch (func)
    {
        case Position: positionFunc = newFunc; break;
        case Velocity: velocityFunc = newFunc; break;
        case Collision: collisionFunc = newFunc; break;
    }
}

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
bool GameObject::has_collision() { return hasCollision; }
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