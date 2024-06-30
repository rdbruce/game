#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

#include "..\entity\Entity.hpp"


// a struct to store all of the information about a scene
struct Scene
{
    // name of the scene
    std::string title = "New Scene";

    // game objects
    std::shared_ptr<GameObject> player = nullptr; // pointer to the player
    std::shared_ptr<GameObject> *heldObject = nullptr; // object the player is holding

    // vector of all the scene's game objects
    std::vector<std::shared_ptr<GameObject>> gameObjects;

    // information about the level's map
    float cell_sideLength; // dimensions of each cell

    int gridX, gridY; // dimensions of grid
    // 2d array of all cells in the map
    std::vector<std::vector<int>> grid;

    // pointers to the scenes above, below, to the left, and right of this scene
    std::shared_ptr<Scene> above = nullptr, below = nullptr, left = nullptr, right = nullptr;

    // constructor
    Scene( 
        GameObject *Player, GameObject *HeldObject, 
        std::vector<GameObject*> GameObjects, 
        float Cell_SideLength, 
        std::vector<std::vector<int>> Grid,
        std::shared_ptr<Scene> Above, std::shared_ptr<Scene> Below,
        std::shared_ptr<Scene> Left, std::shared_ptr<Scene> Right
    );
    Scene();
};





// saving

// save data from a scene into a file
int SaveSceneToFile( Scene scene );

// save attributes of a game object into a file
int SaveGameObjectToFile( std::fstream *file, std::shared_ptr<GameObject> obj );




// loading 

// takes the information stored in a file and loads it into a Scene object
Scene Scene_FromFile( std::string filename );

// creates a game object from the line of a file, and returns a reference to it
std::shared_ptr<GameObject> CreateGameObjectFromFile( std::istringstream *iss );