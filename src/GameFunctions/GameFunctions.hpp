#ifndef GAME_FUNCTIONS_HPP
#define GAME_FUNCTIONS_HPP

#include "../engine/GameMath.hpp"
#include "../entity/Entity.hpp"
#include "../engine/SDLHolder.hpp"
#include "../engine/LTimer.hpp"

using namespace Math;


class GAME
{
    public:

        // initialises the game
        GAME(std::shared_ptr<SDLHolder> g_Holder);
        GAME(const GAME &);

        // takes in user input
        void handleEvent( SDL_Event& e );



        void set_deltaTime( LTimer timer );


        // updates all the game objects
        void update_gameObjects();



        // rendering

        // renders all game objects
        void render_gameObjects();
        // renders the background texture
        void render_background();

        // moves the camera to the player
        void moveCameraToPlayer();



        // accessors
        std::shared_ptr<SDLHolder> get_SDLHolder();
        unsigned char get_inputs();
        unsigned char get_debugTools();
        Uint32 deltaTime();


    private:

        // the camera area
        SDL_Rect camera;


        // current time in game
        float g_time = 0.0f;


        // player can only interact within X cells
        int interactRange_cells = 3;
        // actual interaction distance
        float interactRange = 0.0f;

        unsigned char debugging_tools = 4;
        unsigned char inputKeys = 0;


        // the time elapsed between frames
        Uint32 dTime = 0.0f;
        Uint32 begin_time = 0;


        // dimensions of the current level
        Vector2Int lvlDimensions;

        // a vector of all game objects
        std::vector<std::shared_ptr<GameObject>> gameObjects;
        // reference to the player object
        std::shared_ptr<GameObject> player;


        // developer tools
        int numObjectsinOverlay = 0;



        // SDL variables
        std::shared_ptr<SDLHolder> gHolder;
        std::shared_ptr<LTexture> BGTexture;





        // for instantiating game objects. returns a reference to the object created
        std::shared_ptr<GameObject> Instantiate( EntityType type, Vector2 pos, int hp, std::vector<std::shared_ptr<GameObject>> *vec );
        // std::shared_ptr<GameObject> Instantiate( EntityType type, Vector2 pos, int hp ); // for convenience

        // deletes the specified object
        void Destroy( std::shared_ptr<GameObject> obj, std::vector<std::shared_ptr<GameObject>> *vec );





        // // spawns an item stack with random velocity. return a reference to the item created
        // std::shared_ptr<GameObject> spawnItemStack( EntityType type, Vector2 pos, int count );

        // // when right clicking an item stack, it may be crafted into another type of item
        // // returns a pointer to the new crafted item
        // std::shared_ptr<GameObject> craftItem( std::shared_ptr<GameObject> stack );

        // // when throwing two different item stacks together, they may combine, and produce
        // // a third, different items. returns a pointer to the crafted item
        // std::shared_ptr<GameObject> craftTwoItems( std::shared_ptr<GameObject> stack1, std::shared_ptr<GameObject> stack2 );
};


#endif