#include "Scene.hpp"
#include "../gameObject/GameObject.hpp"

Scene::Scene( std::string filePath, Game *game ) : game(game)
{
    std::ifstream file(filePath);
    if (!file) {
        std::cout << "Couldn't open "<<filePath <<"! :(\n";
    
    } else {

        // get the scene's name from the first line
        std::getline( file, name );


        // get the line of the file as a string
        std::string line;
        std::getline( file, line );
        // convert the string to istringstream
        std::istringstream iss( line );

        int idxPlayer;
        iss >> std::dec>>idxPlayer >> cell_sideLen;


        // next line
        std::getline( file, line );
        iss = std::istringstream( line );

        int n;
        iss >> n;

        // make sure gameObjects is empty
        gameObjects.clear();
        gameObjects = std::vector<std::shared_ptr<GameObject>>(n);
        // for the next n lines, create game objects
        for (int i = 0; i < n; i++)
        {
            // next line
            std::getline( file, line );
            iss = std::istringstream( line );

            // create the game object
            auto obj = CreateGameObjectFromFile( &iss );

            // obj will already have an index, so insert it at that index in lvlObjects
            int id = obj->get_idx();
            gameObjects[id] = obj;

            if (id == idxPlayer) {
                // assign obj to the player pointer
                player = obj;
            }
        }




        // next line
        std::getline( file, line );
        iss = std::istringstream( line );


        iss >> gridDimensions.x >> gridDimensions.y;

        grid.clear();
        grid = std::vector<std::vector<int>>(gridDimensions.x);
        for (int i = 0; i < gridDimensions.x; i++) {
            std::vector<int> vec(gridDimensions.y);

            // next line
            std::getline( file, line );
            iss = std::istringstream( line );

            // read the next ny hex integers into vec
            for (int j = 0; j < gridDimensions.y; j++) {
                iss >> std::hex >> vec[j];
            }

            // add vec into the grid
            grid[i] = vec;
        }

        // close the file
        file.close();
    }
}
Scene::Scene() {}


std::shared_ptr<GameObject> Scene::CreateGameObjectFromFile( std::istringstream *iss )
{
    // all object attributes saved in iss
    Vector2 pos;
    int type, idx, HP;
    
    // read in values from iss
    *iss >> idx >> type >> pos.x >> pos.y >> HP;

    auto obj = std::make_shared<GameObject>(pos, (EntityType)type, idx, HP, game, cell_sideLen);

    return obj;
}






void Scene::Save( std::string directory )
{
    std::string filePath = directory;
    char last = directory[directory.size()-1];
    filePath += (last=='/' || last == '\\')? name+".txt" : '\\'+name+".txt";

    // open the specified file for writing
    std::fstream file;
    file.open(filePath, std::ios::out);
    if (!file) return;

    file << name <<'\n';

    int idxPlayer = (player == nullptr)? -1 : player->get_idx();

    file << std::dec<<idxPlayer <<'\t'<< cell_sideLen <<'\n';


    int n = gameObjects.size();
    file <<n<<'\n';

    for (int i = 0; i < n; i++) {
        SaveGameObjectToFile( &file, gameObjects[i] );
    }

    file << gridDimensions.x <<'\t'<< gridDimensions.y <<'\n';
    for (int i = 0; i < gridDimensions.x; i++) {
        for (int j = 0; i < gridDimensions.y; j++) {
            file << std::hex << grid[i][j] <<'\t';
        }
        file << '\n';
    }
    // close the file
    file.close();
}

void Scene::assignNeighbours(Scene *Above, Scene *Below, Scene *Left, Scene *Right)
{
    above = Above; below = Below; left = Left; right = Right;
}

void Scene::SaveGameObjectToFile( std::fstream *file, std::shared_ptr<GameObject> obj )
{
    // validate the file and the game object
    if (file && obj != nullptr) 
    {
        Vector2 pos = obj->get_pos();
        int idx = obj->get_idx(), type = obj->get_type(), HP = obj->get_hp();

        *file <<std::dec<< idx <<'\t'<< type <<'\t'<< pos.x <<'\t'<< pos.y <<'\t'<< HP <<'\n';
    }
}