#include "Entity.hpp"

// constructor
GameObject::GameObject( Vector2 Pos, Vector2 Velocity, EntityType Type, int Hp, int Idx, GAME *Game )
: pos(Pos), velocity(Velocity), type(Type), hp(Hp), idx(Idx), game(*Game)
{
    switch (type)
    {
        case Player: {
            // assign texture variables
            tex = std::make_shared<LTexture>( game.get_SDLHolder() );
            if (!tex->loadFromFile("../assets/dot.bmp")) {
                printf("Failed to load entity texture!\n");
            }


            // dimensions of the hitbox
            Vector2 size(50.0f, 50.0f);

            // create the hitbox
            hitbox = { int(pos.x-(size.x/2.0f)), int(pos.y-(size.y/2.0f)), (int)size.x, (int)size.y };

            
            // other attributes
            max_hp = 5;
            moveSpeed = 100.0f;

            break;
        }
    }
}





// calls the bahviour functions, updating member variables
void GameObject::update()
{
    positionFunc();
    velocityFunc();
}


void GameObject::positionFunc()
{
    float s = moveSpeed;
    unsigned char inputs = game.get_inputs();

    if ( inputs&16 ) s *= 2.0f;
    if ( game.get_debugTools()&2 ) s *= 2.0f;

    Vector2 vel(
        // horizontal velocity: bit 1=d; move right (+x), bit 3=a; move left (-x)
        (bool(inputs&1)-bool(inputs&4)),
        // vertical velocity: bit 2=s; move down (+y), bit 4=w; move up (-y)
        (bool(inputs&2)-bool(inputs&8))
    );
    // normalise the vector so that players can't walk faster by moving diagonally
    vel.normalise(); 
    vel *= s; // multiply the unit vector by movement speed


    pos += vel * game.deltaTime();
    hitbox.x = pos.x - hitbox.w/2;
    hitbox.y = pos.y - hitbox.h/2;
}


void GameObject::velocityFunc() {}




// shows the entity on the screen relative to the camera
void GameObject::render( int camX, int camY )
{
    // render tex
    tex->render(pos.x - camX, pos.y - camY);
}







// deallocates memory
void GameObject::Delete()
{
    // free the texture
    tex->~LTexture();
}



// mutators
void GameObject::decrement_idx() { idx--; }


// accessors
Vector2 GameObject::get_pos() { return pos; }
Vector2 GameObject::get_velocity() { return velocity; }
Vector2 GameObject::get_size() { return Vector2(hitbox.w, hitbox.h); }
int GameObject::get_idx() { return idx; }