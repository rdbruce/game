#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

#include "../engine/GameMath.hpp"
#include "../engine/LTexture.hpp"
#include "../engine/LWindow.hpp"
#include "../engine/LAudio.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include <iostream>
#include <string>


// forwards declarations (i hate ts)
namespace AStar { class LinkedCell; }
class Game;


using namespace Math;


// entity types
enum EntityType
{
    Player,             // 0

    // enemy types
    ENEMY_MIN,
    Wolf,
    Bird,
    ENEMY_MAX,          

    // end of enemy types

    Bomb,
    Falling_Tree,
    Target,
    Bomb_Explosion_Indicator,

    // item types
    ITEM_MIN,
    Log_Item,       
    Pine_Cone_Item, 
    Plank_Item,     
    Dam_Item,    
    Door_Item,      
    Stone_Item,     
    Berry_Item,
    ITEM_MAX,

    // end of item types

    // NPC types
    NPC_MIN,
    Fox_NPC,
    Bear_NPC,
    NPC_MAX
};



class GameObject
{
    public:

        // constructor
        GameObject( Vector2 pos, EntityType Type, int Idx, int Health, Game *game, int sideLen );
        GameObject();


        // updates member variables
        void update();


        // renders the object onto the window, relative to a camera
        void render( int camX, int camY, Uint8 alpha = 255 );


        bool is_item();
        bool is_enemy();


        // changes to held item position func
        void make_held();
        // changes back to thrownItemPositionFunc
        void make_thrown( Vector2 newVelocity, Vector2 newAcceleration );

        // mutators
        void    set_pos( Vector2 newPos );
        void    set_velocity( Vector2 newVelocity );
        void    set_acceleration( Vector2 newAcceleration );
        void    set_HP( int newHP );
        void    set_timer( float t );
        void    decrement_idx();


        // accessors
        Vector2     get_pos();
        Vector2     get_velocity();
        Vector2     get_acceleration();
        int         get_idx();
        EntityType  get_type();
        int         get_hp();
        int         get_maxHP();
        int         get_damage();
        float       get_moveSpeed();
        float       get_radius();
        float       get_timer();
        bool        has_collision();
        bool        is_held();
        bool        is_NPC();
        SDL_Rect    get_hitbox();
        Vector2Int  get_size();
        Vector2Int  get_cell();
        // accesor functions for game variables
        Uint8       get_inputKeys();
        float       get_deltaTime();
        Vector2Int  get_mapDimensions();
        Vector2     get_player_pos();



    private:

        // position of the centre of the object's hitbox
        Vector2 pos;
        Vector2 velocity = Vector2_Zero;
        Vector2 acceleration = Vector2_Zero;

        // what kind of entity it is
        EntityType type;
        // the entity's index in a gameObjects vector
        int idx;


        // how fast the object can move
        float moveSpeed = 0.0f;


        // the objects current health, and max health
        int hp, max_hp;
        // how much damage the entity deals
        int damage = 0;

        // used for entities with functions dependent on time intervals
        float timer = 0.0f;

        // position and size of the hitbox
        SDL_Rect hitbox;
        // circular radius used for detecting game object collisions
        float radius = 0.0f;
        // whether or not the object performs collision checks
        bool hasCollision;


        // a linked list of cells used for pathfinding
        AStar::LinkedCell *path = nullptr;
        // how many nodes are in the path linked list
        int pathfindingLength = 0;

        
        // i don't know how, I don't know why, but removing this variable may cause the game to crash
        std::string apparently_very_important_variable = "LEAVE THIS";


        // the image to be drawn
        std::shared_ptr<LTexture> tex, altTex;

        // sounds 
        std::shared_ptr<LAudio> deathSound = nullptr;

        // a reference to the game itself
        Game *game = nullptr;


        // behaviour functions
        void (GameObject::*positionFunc) (void);
        void (GameObject::*velocityFunc) (void);
        void (GameObject::*collisionFunc)(void);
        void (GameObject::*renderingFunc)( int, int, Uint8 );







        //////////////////////////////////////////////////////////////
        //                                                          //
        //          FOLLOWING ARE ALL OF THE BEHAVIOUR              //
        //          FUNCTIONS FOR ENTITIES IN THE GAME              //
        //                                                          //
        //    To change an entity's  behaviour, assign a member     //
        //    function   (positionFunc,   for  instance),  with     //
        //    "positionFunc = &GameObject::playerPositionFunc;"     //
        //                                                          //
        //    Alternatively, some functions, like 'make_held()'     //
        //         can also update behaviour functions.             //
        //                                                          //
        //////////////////////////////////////////////////////////////



        // velocity functions

        // gives wolf velocity in the direction of the player
        void wolfVelocityFunc();
        // a jumping attack for the wolf
        void wolfJumpAttack();
        // moves to the centre of the map without collision until it gets into the map
        void wolfSpawningFunc();

        // tries to return back to a constant point. retreats upon nightfall
        void foxVelocityFunc();


        // adds object's acceleration to their velocity
        void defaultVelocityFunc();
        // adds acceleration to velocity, until the object comes to a stop
        void deccelerateVelocityFunc();

        // draws a unit vector to the player and moves towards it
        void moveDirectlyToPlayer();
        void retreatVelocityFunc();




        // position functions

        void playerPositionFunc();

        // attempt to move back to a specified point
        void foxPositionFunc();

        // adds velocity to current position
        void defaultPositionFunc();
        // snaps the bear in place
        void bearPositionFunc();
        // trees don't move, snap their position to the position they spawned
        void fallingTreePositionFunc();

        // holds the item stack a constant distance away from the player's centre
        void heldItemPositionFunc();
        // adds velocity to the position. when the velocity reaches zero, set acceleration to zero.
        void thrownItemPositionFunc();

        // flies along a fixed arc
        void birdPositionFunc();
        // drops to the ground, then explodes
        void bombPositionFunc();



        // pathfinding!!!!
    
        // finds the best path to the player
        void pathfindToPlayer( int searchDepth );

        void wolfUpdatePathfinding();
        void wolfWalkToNextCell();



        // functions that make switching between these functions easier
        
        // switches a wolf into it's jump attack
        void wolfEnterJumpAttack();
        // enters into the retreating velocity function
        void beginRetreat();
        // blows up bombs, damaging nearby cells and entities
        void explodeBomb();






        // collision functions

        // detects collisions with world borders, walls, and other game objects
        void defaultCollisionFunction();

        // similar to default, but lets the player load different areas when walking off the map
        void playerCollisionFunction();

        
        // for an item sitting still, collide with non-items normally, but when colliding
        // with another item, combine stacks, if possible
        void stationaryItemCollisionFunction();



        // pushes apart gameobjects that are too close to itself
        void defaultHandleCollisionsWithGameObjects();
        // pushes apart objects that are too close to itself,
        // or combine stacks with other items
        void itemsHandleCollisionsWithGameObjects();




        // makes sure the object cannot walk out of bounds
        bool defaultCollideWithWorldBorders();
        // loads different areas based on where the player walks
        void playerCollideWithWorldBorders();

        // checks if the hitbox is within a wall, and pushes it out
        void defaultHandleSideCollisionsWithWalls();
        void defaultHandleCornerCollisionsWithWalls();

        



        // rendering functions
        void defaultRenderFunc( int camX, int camY, Uint8 alpha = 255 );
        void fallingTreeRenderFunc( int camX, int camY, Uint8 alpha = 255 );
        void itemRenderFunc( int camX, int camY, Uint8 alpha = 255 );
        void targetRenderFunc( int camX, int camY, Uint8 alpha = 255 );

        void foxRenderFunc( int camX, int camY, Uint8 alpha = 255 );
        void bearRenderFunc( int camX, int camY, Uint8 alpha = 255 );
        void playerRenderFunc( int camx, int camY, Uint8 alpha = 255 );
};

#endif