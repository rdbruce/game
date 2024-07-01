#include "GameObject.hpp"

// constructor
GameObject::GameObject( Vector2 pos, Game *game ) : pos(pos), game(game)
{
    // assign the texture
    tex = LTexture( game->window );
    if (!tex.loadFromFile("../../assets/Bert.png")) {
        printf("Failed to load entity texture!\n");
    }


    // set up the hitbox
    Vector2Int size(25, 25);
    Vector2Int p = Vector2Int(pos.x, pos.y) - (size/2);
    hitbox = { p.x, p.y, size.x, size.y };

    moveSpeed = 200.0f;
}


void GameObject::render( int camX, int camY )
{
    Vector2Int p( pos.x-camX, pos.y-camY );
    tex.render( p.x, p.y );
}


void GameObject::update()
{
    
    float s = moveSpeed; // store in a local variable, to save time loading from memory
    Uint8 inputs = game->inputKeys; // input keys being held

    Vector2 vel(
        // horizontal velocity: bit 1=d; move right (+x), bit 3=a; move left (-x)
        (bool(inputs&1)-bool(inputs&4)),
        // vertical velocity: bit 2=s; move down (+y), bit 4=w; move up (-y)
        (bool(inputs&2)-bool(inputs&8))
    );

    // normalise the vector so that players can't walk faster by moving diagonally
    vel.normalise(); 
    vel *= s; // multiply the unit vector by movement speed

    pos += vel * game->deltaTime;
    hitbox.x = pos.x-(hitbox.w/2);
    hitbox.y = pos.y-(hitbox.h/2);
}