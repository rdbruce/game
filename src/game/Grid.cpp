#include "Game.hpp"
#include "CellTypes.hpp"
#include "Scene.hpp"




bool Game::is_barrier( Vector2Int cell ) {
    // validate the cell 
    if (cell.x < 0 || cell.y < 0 || cell.x>=currLevel->gridDimensions.x || cell.y>=currLevel->gridDimensions.y) return false;
    return currLevel->grid[cell.x][cell.y]&BARRIER; 
}



// place an object into a cell in the global grid
int Game::PlaceObjectInCell(Vector2Int cell, int objType, bool playerPlacement)
{
    // cell is out of bounds and doesn't exist
    if (cell.x<0 || cell.x>=currLevel->gridDimensions.x ||
        cell.y<0 || cell.y>=currLevel->gridDimensions.y) return -1;

    std::vector<std::vector<int>> grid = currLevel->grid;
    // a rect representing the area of the cell in the map

    int sideLen = currLevel->cell_sideLen;
    int x = cell.x * sideLen, y = cell.y * sideLen;
    SDL_Rect cellRect = { x, y, sideLen, sideLen };

    switch (objType & 255) // just the first byte, containing cell ID
    {

        case 1: // log

            // cell is occupied, AND the placement was made by player, not loading
            if (grid[cell.x][cell.y]&0x4000 && playerPlacement) return -2;

            // draw the background of the cell first
            if (grid[cell.x][cell.y]&WATER) tEditor.renderTextureToTexture(BGTexture, waterTex, &cellRect);
            else tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);

            // draw a log into the cell
            tEditor.renderTextureToTexture(BGTexture, logTex, &cellRect);

            // update the grid value to represent a log
            grid[cell.x][cell.y] |= LOG;
            break;


        case 2: // bridge

            // cell is occupied, AND the placement was made by player, not loading
            if (grid[cell.x][cell.y]&0x4000 && playerPlacement) return -2;

            // draw the background of the cell first
            if (grid[cell.x][cell.y]&WATER) tEditor.renderTextureToTexture(BGTexture, waterTex, &cellRect);
            else tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);

            // draw a log into the cell
            tEditor.renderTextureToTexture(BGTexture, logTex, &cellRect);

            // update the grid value to represent a bridge
            grid[cell.x][cell.y] |= BRIDGE;
            grid[cell.x][cell.y] &= ~BARRIER; // turn off barrier bit
            break;

        case 3: // tree
            // come back here
            break;

        
        case 4: // stump
            tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);
            // draw a stump into the cell
            tEditor.renderTextureToTexture(BGTexture, stumpTex, &cellRect);
            // only occurs when a tree gets removed, thus the cell could only have been TREE prior.
            // so, there is no issue just settig the cell to STUMP, no need for |=
            grid[cell.x][cell.y] = STUMP;
            break;


        case 5: // sapling
            // cell is occupied or water, AND the placement was made by player, not loading
            if (grid[cell.x][cell.y]&0x5000 && playerPlacement) return -2;

            tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);
            // draw a sapling into the cell
            tEditor.renderTextureToTexture(BGTexture, saplingTex, &cellRect);

            // update the grid to represent a sapling
            grid[cell.x][cell.y] |= SAPLING;
            break;


        case 6: { // world border
            // make the cell empty (black)
            SDL_Texture *borderTex = SDL_CreateTexture(window->gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, sideLen, sideLen);
            tEditor.renderTextureToTexture(BGTexture, borderTex, &cellRect);
            // update the grid to represent a border
            grid[cell.x][cell.y] = BORDER;
            break;
        }

        case 7: // closed door
            // cell is occupied or water, AND the placement was made by player, not loading
            if (grid[cell.x][cell.y]&0x5000 && playerPlacement) {
                if (grid[cell.x][cell.y]&255 != 8) return -2;
                else grid[cell.x][cell.y] = CLOSED_DOOR;
            } else grid[cell.x][cell.y] |= CLOSED_DOOR;

            // draw the empty tile first
            tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);
            tEditor.renderTextureToTexture(BGTexture, closed_doorTex, &cellRect);
            break;

        case 8: // open door
            // cell is occupied or water, AND the placement was made by player, not loading
            if (grid[cell.x][cell.y]&0x5000 && playerPlacement) {
                if ((grid[cell.x][cell.y]&255) != 7) return -2;
                else grid[cell.x][cell.y] = OPEN_DOOR;
            } else grid[cell.x][cell.y] |= OPEN_DOOR;

            // draw the empty tile first
            tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);
            tEditor.renderTextureToTexture(BGTexture, open_doorTex, &cellRect);
            break;


        case EMPTY:
        {
            // also come back here :3


            // remove building in the cell
            // if it IS indestructible or NOT occupied, return
            if (grid[cell.x][cell.y]&0x8000) return -3;

            // first byte represents info about the cell, this is the object being removed
            switch (grid[cell.x][cell.y]&255)
            {
                case 1: { // log
                    // spawn a log
                    Vector2 pos(x, y);
                    spawnItemStack(Log_Item, pos, 1);

                    if (grid[cell.x][cell.y]&WATER) tEditor.renderTextureToTexture(BGTexture, waterTex, &cellRect);
                    else tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);

                    break;
                }

                case 2: { // bridge
                    // spawn a bridge
                    Vector2 pos(x, y);
                    spawnItemStack(Bridge_Item, pos, 1);

                    if (grid[cell.x][cell.y]&WATER) tEditor.renderTextureToTexture(BGTexture, waterTex, &cellRect);
                    else tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);

                    break;
                }

                case 3: { // tree
                    // also also come back here :3
                    break;
                }

                case 4: { // stump
                    // spawn a log
                    Vector2 pos(x, y);
                    spawnItemStack(Log_Item, pos, 1);

                    if (grid[cell.x][cell.y]&WATER) tEditor.renderTextureToTexture(BGTexture, waterTex, &cellRect);
                    else tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);

                    break;
                }

                case 5: { // sapling
                    // spawn a pine cone
                    Vector2 pos(x, y);
                    spawnItemStack(Pine_Cone_Item, pos, 1);

                    if (grid[cell.x][cell.y]&WATER) tEditor.renderTextureToTexture(BGTexture, waterTex, &cellRect);
                    else tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);

                    break;
                }

                case 6: return 0; // world border cannot be modified, exit the function

                case 7: { // closed door
                    // spawn a door
                    Vector2 pos(x, y);
                    spawnItemStack(Door_Item, pos, 1);

                    if (grid[cell.x][cell.y]&WATER) tEditor.renderTextureToTexture(BGTexture, waterTex, &cellRect);
                    else tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);

                    break;
                }

                case 8: { // open door
                    // spawn a door
                    Vector2 pos(x, y);
                    spawnItemStack(Door_Item, pos, 1);

                    if (grid[cell.x][cell.y]&WATER) tEditor.renderTextureToTexture(BGTexture, waterTex, &cellRect);
                    else tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);

                    break;
                }

                default:
                    if (grid[cell.x][cell.y]&WATER) tEditor.renderTextureToTexture(BGTexture, waterTex, &cellRect);
                    else tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);

                    break;
            }
            // preserve bits 16 (indestructible), and 13, 17-24 (water), 
            // all others to 0
            // bit 14: on if water (bit 13), off otherwise
            grid[cell.x][cell.y] &= 0xFF9000;
            if (grid[cell.x][cell.y]&WATER) grid[cell.x][cell.y] |= 0x2000;
            else grid[cell.x][cell.y] &= ~0x2000;
            break;
        }
        currLevel->grid = grid;
    }
    return 0;
}


// deals a specified amount of damage to a cell
void Game::damageCell( Vector2Int cell, int damage )
{
    if (damage == 0) return;
    int num = currLevel->grid[cell.x][cell.y];

    // nothing in the cell to damage, or it cannot be damaged
    if ((num&255) == 0 || (num&CAN_DIE) == 0) return;

    // find the current amount of health in the cell
    // bit shift it to represent as a regular value
    int health = (num&HEALTH)>>8;

    // subtract the amount of damage dealth
    health -= damage;

    // if the health is now non-positive, destroy the cell
    if (health <= 0) {
        PlaceObjectInCell(cell, EMPTY, true);
    } else {
        // bit shift health to be in the right position
        health <<= 8;
        // reset the health bits of the cell
        num &= ~HEALTH;
        // set the health bits to the new value
        num |= health;
        currLevel->grid[cell.x][cell.y] = num;
    }
}


void Game::initialise_BGTexture() {
    int sideLen = currLevel->cell_sideLen;
    map = { 0, 0, currLevel->gridDimensions.x*sideLen, currLevel->gridDimensions.y*sideLen };
    BGTexture = tEditor.createEmptyTexture(map.w, map.h, window);
    for (int x = 0; x < currLevel->gridDimensions.x; x++) {
        for (int y = 0; y < currLevel->gridDimensions.y; y++) {
            PlaceObjectInCell(Vector2Int(x, y), currLevel->grid[x][y], false);
        }
    }
}