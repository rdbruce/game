#ifndef GAME_HPP
#define GAME_HPP

#include "../engine/LTexture.hpp"
#include "../engine/LTimer.hpp"
#include "../engine/LWindow.hpp"
#include "../engine/GameMath.hpp"
#include "../engine/TextureManipulator.hpp"
#include "../gameObject/GameObject.hpp"
#include "Scene.hpp"
#include "CellTypes.hpp"
#include "Dialogue.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_events.h>

#include <ctime>
#include <vector>
#include <random>

using namespace Math;



class Game
{
    // declare GameObject as a friend of Game, so the private members can be accessed
    friend class GameObject;
    friend class EntityFunctions;

    public:

        // constructor
        Game( std::shared_ptr<LWindow> Window );


        // handle events like player input
        void handle_events( SDL_Event& e );

        // updates all the game objects
        void update_gameobjects();



        // rendering functions

        void center_camera_on_player();
        // renders the scene's background
        void render_background();
        // renders all the game objects
        void render_gameobjects();
        // renders trees
        void render_overlay();
        // shows the current frame rate
        void render_framerate();


        // finds the time elapsed between frames
        void update_deltaTime();

        // finds the position of the mouse (on the map)
        Vector2 find_mouse_pos();


        // adds a game object and returns a pointer to it
        std::shared_ptr<GameObject> Instantiate( Vector2 pos, int type, int hp, Scene *level );
        std::shared_ptr<GameObject> Instantiate( Vector2 pos, int type, int hp ); // for convenience

        // deletes the specified object
        void Destroy( std::shared_ptr<GameObject> obj, std::vector<std::shared_ptr<GameObject>> *vec );
        void Destroy( std::shared_ptr<GameObject> obj ); // for convenience

        // spawns an item stack with random velocity. returns a pointer to the item created
        std::shared_ptr<GameObject> spawnItemStack( int type, Vector2 pos, int count );

        // when throwing two different item stacks together, they may combine, and produce
        // a third, different item. returns a pointer to the crafted item
        std::shared_ptr<GameObject> craftTwoItems( std::shared_ptr<GameObject> item1, std::shared_ptr<GameObject> item2 );
        // when right clicking an item stack, it may be crafted into another type of item
        std::shared_ptr<GameObject> craftItem( std::shared_ptr<GameObject> item );


        // moves the player into the specified level, and makes said level the active scene
        void movePlayerToLevel( Scene *level, Vector2 newPlayerPos );

        // moves an entity into the specified level
        std::shared_ptr<GameObject> moveEntityToLevel( std::shared_ptr<GameObject> obj, Scene *level, Vector2 newPos );

        // deals a specified amount of damage to a cell
        void damageCell( Vector2Int cell, int damage );

        // toggles day/night, when the appropriate amount of time has passed
        void dayNightCycle();

        // updates any cells dependent on time
        void update_cells();

        // attempts to spawn enemies
        void attempt_enemy_spawn();

        // sets the current dialogue to the specified option
        void enter_dialogue( Dialogue newDialogue );

        // if the player is currently in dialogue, this function will take player input to see which dialogue option they select
        void handle_dialogue( int e );

        // saves all scenes to file
        void save_game();


    private:

        // returns true if the type represents an Item
        bool is_item( int type );
        // returns true if the specified cell is a barrier
        bool is_barrier( Vector2Int cell );


        // for tracking player input
        // tracks which keys the player has held down
        Uint8 inputKeys = 0;
        // determines how far away the player can interact with things
        float interactRange = 0.0f;

        // tracks which dialogue (if any) the player is currently engaged in
        Dialogue currDialogue = None;



        // dimensions of the current map
        SDL_Rect map;


        // the time elapsed between frames
        float deltaTime = 0.0f;
        clock_t begin_time = 0; // used for calculating deltaTime
        int fps;
        std::unique_ptr<LTexture> fpsTex;
        // tracking the time of day/night
        float g_time = 0.0f;
        // whether or not it is currently night time
        bool isNight;


        // position and dimensions of the camera
        SDL_Rect camera;


        // all the levels in the game
        Scene *currLevel;
        Scene Base;
        Scene Woods;
        Scene Woods2;
        // true when switching between levels
        bool switching_scenes = false;

        

        // images/textures

        // used for editing textures
        TextureManipulator tEditor;

        // loads all of the games textures from file
        void load_textures();

        // loads levels from files, chooses one to be the active level
        void load_levels();

        // background texture
        std::shared_ptr<LTexture> BGTexture = nullptr, overlayTexture = nullptr, CRT_Tex = nullptr, CRT = nullptr;
        std::shared_ptr<LTexture> Bert = nullptr;
        std::shared_ptr<LTexture>   logTex, bridgeTex, waterTex, grassTex, treeTex,
                                    stumpTex, saplingTex, playerTex, wolfTex, falling_treeTex,
                                    pine_coneTex, plankTex, foxTex,
                                    shoreline0Tex, shoreline1Tex, shoreline2Tex, shoreline3Tex,
                                    shoreline4Tex, closed_doorTex, open_doorTex;
        
        // the window the game will be rendered to
        std::shared_ptr<LWindow> window = nullptr;


        // updates the CRT_texture, making it brighter/darker depending on time
        void update_CRT();


        // functions for editing the grid (defined in Grid.cpp NOT Game.cpp!!!!)

        // updates any cells dependent on time in a level
        void update_cells( Scene *level );

        // place an object into a cell in the global grid
        int PlaceObjectInCell(Vector2Int cell, int objType, bool playerPlacement, Scene *level = NULL);

        // renders a tree texture into the overlay texture
        void AddTreeToOverlay( Vector2Int cell );
        // removes a tree from the overlay texture
        void RemoveTreeFromOverlay( Vector2Int cell );

        // fills a cell with the water texture, as well as any shoreline
        void DrawWaterToCell( Vector2Int cell, SDL_Rect cellRect );


        // renders every cell in the level, to be used when loading levels
        void initialise_BGTexture();


        // returns true if the coordinates are within the rect
        bool isInRegion( Vector2 p, SDL_Rect rect );

        // returns the building corresponding to a given item type
        int get_building( EntityType type );

        // if the player is holding an object, throw it, otherwise try to pick one up
        void leftClickFunc();
        // if the player IS holding an item, attempt to place it, otherwise, do nothing (for now c:)
        void rightClickFunc();



        // sets the selected object to be held by the player
        void setHeldObject( std::shared_ptr<GameObject> obj );
        // throw the object the player is currently holding
        void throwHeldObject();


        // spawns a wolf at a random location on the maps edge
        std::shared_ptr<GameObject> spawnWolf();
};

#endif