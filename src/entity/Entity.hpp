#ifndef ENTITY_HPP
#define ENTITY_HPP


#include <SDL.h>
#include <SDL_image.h>

#include <memory>
#include <vector>

#include "../engine/LTexture.hpp"
#include "../engine/GameMath.hpp"
#include "AStarPathfinding.hpp"
#include "../GameFunctions/GameFunctions.hpp"


using namespace Math;

// forward declaration of GAME class
class GAME;



// entity types
enum EntityType
{
    Player, // 0
    Wolf,    // 1
    Falling_Tree, // 2

    // item stack entities
    Log_Item, // 3
    Pine_Cone_Item, // 4
    Plank_Item, // 5
    Bridge_Item, // 6
    Door_Item, // 7
};


// facing directions (for animations object)
enum Direction
{
    Front, // 0
    Back, // 1
    Left, // 2
    Right // 3
};








// object to store Textures for game objects with animation
class Animations 
{
    public:

        // returns the pointer to an LTexture selected from the object
        LTexture get_texture( Direction facing );

        // constructor
        Animations(
            std::vector<LTexture> Front,
            std::vector<LTexture> Back,
            std::vector<LTexture> Left,
            std::vector<LTexture> Right,
            float Front_Time, float Back_Time, 
            float Left_time, float Right_time
        );
        Animations();


    private:
        // sets of images, all of the images in one vector 
        // will be shows sequentially to play an animation
        std::vector<LTexture> front, back, left, right;

        // the amount of time needed to pass between each image in each vector
        float front_interval, back_interval, left_interval, right_interval;
};









class GameObject 
{
    public:

        // constructor
        GameObject( Vector2 Pos, Vector2 Velocity, EntityType Type, int Hp, int Idx, GAME* Game );

        // calls the bahviour functions, updating member variables
        void update();

        // shows the entity on the screen relative to the camera
        void render( int camX, int camY );


        // mutators
        void decrement_idx();


        // accessors
        Vector2 get_pos();
        Vector2 get_velocity();
        Vector2 get_size();
        int     get_idx();

        // deallocates memory
        void Delete();


    private:

        // position of the centre of the entity
        Vector2 pos;

        // velocity of the entity
        Vector2 velocity;

        // texture used to render the entity
        std::shared_ptr<LTexture> tex;

        // what type of entity it is
        EntityType type;

        // index in the gameObjects vector
        int idx;

        // the object's health and maxHP
        int hp, max_hp;

        // how fast the object moves
        float moveSpeed;


        // position and dimensions of the hitbox
        SDL_Rect hitbox;


        void positionFunc();
        void velocityFunc();


        GAME& game; // a reference to the game object
};

#endif