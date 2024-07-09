#include "Game.hpp"
#include "CellTypes.hpp"
#include "Scene.hpp"




bool Game::is_barrier( Vector2Int cell ) {
    // validate the cell 
    if (cell.x < 0 || cell.y < 0 || cell.x>=currLevel->gridDimensions.x || cell.y>=currLevel->gridDimensions.y) return false;
    return currLevel->grid[cell.x][cell.y]&BARRIER; 
}



// place an object into a cell in the global grid
int Game::PlaceObjectInCell(Vector2Int cell, int objType, bool playerPlacement, Scene *level)
{
    if (level == NULL) level = currLevel;
    bool draw = level == currLevel;

    // cell is out of bounds and doesn't exist
    if (cell.x<0 || cell.x>=level->gridDimensions.x ||
        cell.y<0 || cell.y>=level->gridDimensions.y) return -1;

    int num = level->grid[cell.x][cell.y];
    // a rect representing the area of the cell in the map

    int sideLen = level->cell_sideLen;
    int x = cell.x * sideLen, y = cell.y * sideLen;
    SDL_Rect cellRect = { x, y, sideLen, sideLen };

    switch (objType & 255) // just the first byte, containing cell ID
    {

        case 1: // log

            // cell is occupied, AND the placement was made by player, not loading
            if (num&0x4000 && playerPlacement) return -2;

            if (draw) {
                // draw the background of the cell first
                if (num&WATER) DrawWaterToCell(cell, cellRect);
                else tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);

                // draw a log into the cell
                tEditor.renderTextureToTexture(BGTexture, logTex, &cellRect);
            }

            // update the grid value to represent a log
            num |= LOG;
            break;


        case 2: // bridge

            // cell is occupied, AND the placement was made by player, not loading
            if (num&0x4000 && playerPlacement) return -2;

            if (draw) {
                // draw the background of the cell first
                if (num&WATER) DrawWaterToCell(cell, cellRect);
                else tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);

                // draw a log into the cell
                tEditor.renderTextureToTexture(BGTexture, bridgeTex, &cellRect);
            }

            // update the grid value to represent a bridge
            num |= BRIDGE;
            num &= ~BARRIER; // turn off barrier bit
            break;

        case 3: // tree
            if (draw) {
                // empty grass tile where the base is
                tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);
                // add a tree to the overlay layer
                AddTreeToOverlay(cell);
            }
            
            // set the cell space to be a tree
            // use = (not |=) because previous data SHOULD be overwritten; if the tree grew
            // from a sapling, the cell should no longer contain any data about the sapling
            num = TREE;
            break;

        
        case 4: // stump
            if (draw) {
                tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);
                // draw a stump into the cell
                tEditor.renderTextureToTexture(BGTexture, stumpTex, &cellRect);
            }
            // only occurs when a tree gets removed, thus the cell could only have been TREE prior.
            // so, there is no issue just settig the cell to STUMP, no need for |=
            num = STUMP;
            break;


        case 5: // sapling
            // cell is occupied or water, AND the placement was made by player, not loading
            if (num&0x5000 && playerPlacement) return -2;

            if (draw) {
                tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);
                // draw a sapling into the cell
                tEditor.renderTextureToTexture(BGTexture, saplingTex, &cellRect);
            }

            // update the grid to represent a sapling
            num |= SAPLING;
            break;


        case 6: { // world border
            if (draw) {
                // make the cell empty (black)
                std::shared_ptr<LTexture> black = tEditor.createSolidColour(sideLen, sideLen, 0x000000FF, window);
                tEditor.renderTextureToTexture(BGTexture, black, &cellRect);
            }
            // update the grid to represent a border
            num = BORDER;
            break;
        }

        case 7: // closed door
            // cell is occupied or water, AND the placement was made by player, not loading
            if (num&0x5000 && playerPlacement) {
                if (num&255 != 8) return -2;
                else num = CLOSED_DOOR;
            } else num |= CLOSED_DOOR;

            if (draw) {
                // draw the empty tile first
                tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);
                tEditor.renderTextureToTexture(BGTexture, closed_doorTex, &cellRect);
            }
            break;

        case 8: // open door
            // cell is occupied or water, AND the placement was made by player, not loading
            if (num&0x5000 && playerPlacement) {
                if ((num&255) != 7) return -2;
                else num = OPEN_DOOR;
            } else num |= OPEN_DOOR;

            if (draw) {
                // draw the empty tile first
                tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);
                tEditor.renderTextureToTexture(BGTexture, open_doorTex, &cellRect);
            }
            break;


        case EMPTY:
        {
            // cannot remove cells while holding an object
            if (level->held != nullptr && playerPlacement) return -4;

            // remove building in the cell
            // if it IS indestructible or NOT occupied, return
            if (num&0x8000 && playerPlacement) return -3;

            // first byte represents info about the cell, this is the object being removed
            switch (num&255)
            {
                case 1: { // log
                    // spawn a log
                    Vector2 pos(x-10.0f+(cellRect.w/2), y-10.0f+(cellRect.h/2));
                    spawnItemStack(Log_Item, pos, 1);

                    if (draw) {
                        if (num&WATER) DrawWaterToCell(cell, cellRect);
                        else tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);
                    }

                    break;
                }

                case 2: { // bridge
                    // spawn a bridge
                    Vector2 pos(x-10.0f+(cellRect.w/2), y-10.0f+(cellRect.h/2));
                    spawnItemStack(Bridge_Item, pos, 1);

                    if (draw) {
                        if (num&WATER) DrawWaterToCell(cell, cellRect);
                        else tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);
                    }

                    break;
                }

                case 3: { // tree
                    // remove tree from the grid
                    level->grid[cell.x][cell.y] &= 0xFF9000;
                    if (draw) {
                        // remove tree image from the overlay layer
                        RemoveTreeFromOverlay(cell);
                    }

                    // spawn a falling tree in its place
                    Vector2 pos(((float)cell.x+0.5f)*sideLen, ((float)cell.y-4.5f)*sideLen);
                    Instantiate(pos, Falling_Tree, 1, level);

                    // place a stump where the tree once stood
                    // everything else handled in the STUMP case. don't break, just return
                    return PlaceObjectInCell(cell, STUMP, false, level);
                }

                case 4: { // stump
                    // spawn a log
                    Vector2 pos(x-10.0f+(cellRect.w/2), y-10.0f+(cellRect.h/2));
                    spawnItemStack(Log_Item, pos, 1);

                    if (draw) {
                        if (num&WATER) DrawWaterToCell(cell, cellRect);
                        else tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);
                    }

                    break;
                }

                case 5: { // sapling
                    // spawn a pine cone
                    Vector2 pos(x-10.0f+(cellRect.w/2), y-10.0f+(cellRect.h/2));
                    spawnItemStack(Pine_Cone_Item, pos, 1);

                    if (draw) {
                        if (num&WATER) DrawWaterToCell(cell, cellRect);
                        else tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);
                    }

                    break;
                }

                case 6: return 0; // world border cannot be modified, exit the function

                case 7: { // closed door
                    // spawn a door
                    Vector2 pos(x-10.0f+(cellRect.w/2), y-10.0f+(cellRect.h/2));
                    spawnItemStack(Door_Item, pos, 1);

                    if (draw) {
                        if (num&WATER) DrawWaterToCell(cell, cellRect);
                        else tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);
                    }

                    break;
                }

                case 8: { // open door
                    // spawn a door
                    Vector2 pos(x-10.0f+(cellRect.w/2), y-10.0f+(cellRect.h/2));
                    spawnItemStack(Door_Item, pos, 1);

                    if (draw) {
                        if (num&WATER) DrawWaterToCell(cell, cellRect);
                        else tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);
                    }

                    break;
                }

                default:
                    if (draw) {
                        if (num&WATER) DrawWaterToCell(cell, cellRect);
                        else tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);
                    }

                    break;
            }
            // preserve bits 16 (indestructible), and 13, 17-24 (water), 
            // all others to 0
            // bit 14: on if water (bit 13), off otherwise
            num &= 0xFF9000;
            if (num&WATER && !(num&IS_DRIED)) num |= BARRIER;
            else num &= ~BARRIER;
            break;
        }
    }
    level->grid[cell.x][cell.y] = num;
    return 0;
}

void Game::update_cells( Scene *level )
{
    Vector2Int dimensions = level->gridDimensions;

    for (int x = 0; x < dimensions.x; x++) {
        for (int y = 0; y < dimensions.y; y++) {
            int type = level->grid[x][y] & 255;

            switch (type)
            {
                case 5: // sapling
                    // saplings grow at the beginning of each day
                    if (g_time == 0.0f && !isNight) {
                        PlaceObjectInCell(Vector2Int(x,y), TREE, false, level);
                    }
                    break;
            }
        }
    }
}

void Game::update_cells()
{
    // update every 2 seconds
    if (((int(g_time)%2) == 0 && (int(g_time-deltaTime)%2) != 0) || g_time == 0.0f) {
        update_cells(&Base);
        update_cells(&Woods);
        update_cells(&Woods2);
    }
}


void Game::AddTreeToOverlay( Vector2Int cell )
{
    int sideLen = currLevel->cell_sideLen;
    SDL_Rect rect = {(cell.x-1)*sideLen, (cell.y-9)*sideLen, sideLen*3, sideLen*10};
    tEditor.renderTextureToTexture(overlayTexture, treeTex, &rect);
}

void Game::RemoveTreeFromOverlay( Vector2Int cell )
{
    // reset the overlay texture
    int sideLen = currLevel->cell_sideLen;
    int nx = currLevel->gridDimensions.x, ny = currLevel->gridDimensions.y;
    map = { 0, 0, nx*sideLen, ny*sideLen };
    overlayTexture = tEditor.createEmptyTexture(map.w, map.h, window);
    for (int x = 0; x < nx; x++) {
        for (int y = 0; y < ny; y++) {
            int type = currLevel->grid[x][y]&255;
            if (type == 3) AddTreeToOverlay( Vector2Int(x, y) );
        }
    }
}


void Game::DrawWaterToCell( Vector2Int cell, SDL_Rect cellRect )
{
    // dimensions of grid -1
    Vector2Int dimensions = currLevel->gridDimensions - Vector2Int_One;
    // ensure the cell is in bounds
    if (cell.x<0 || cell.x>dimensions.x || cell.y<0 || cell.y>dimensions.y) return;

    // get the grid
    std::vector<std::vector<int>> grid = currLevel->grid;
    // ensure the cell is actuall water
    if (!(grid[cell.x][cell.y]&WATER)) return;

    // draw the base water image to the cell
    if (grid[cell.x][cell.y]&IS_DRIED) tEditor.renderTextureToTexture(BGTexture, dirtTex, &cellRect);
    else tEditor.renderTextureToTexture(BGTexture, waterTex, &cellRect);

    // sample the four adjacent cells to see where shoreline should be drawn
    Uint8 shoreLocations = 0b00000000; // lrLR TBLR
    /*
        METHOD:
        
        the eight bits of shoreLocations represent the following:

        1: there is a shoreline on the TOP side of the cell
        2: there is a shoreline on the RIGHT side of the cell
        3: there is a shoreline on the BOTTOM of the cell
        4: there is a shoreline on the LEFT of the cell

        5: there is shore in the TOP LEFT corner of the cell
        6: there is shore in the TOP RIGHT corner of the cell
        7: there is shore in the BOTTOM RIGHT corner of the cell
        8: there is shore in the BOTTOM LEFT corner of the cell

        run checks on the surrounding cells to see where there is and isn't land, in order to determine
        where there should and shouldn't be shore, setting these bits
    */
    // check the cell above
    if (cell.y >= 1) {
        int num = grid[cell.x][cell.y-1];
        if (!(num&WATER || num==BORDER)) shoreLocations |= 0x01;
    }
    // check the cell to the right
    if (cell.x < dimensions.x) {
        int num = grid[cell.x+1][cell.y];
        if (!(num&WATER || num==BORDER)) shoreLocations |= 0x02;
    }
    // check the cell below
    if (cell.y < dimensions.y) {
        int num = grid[cell.x][cell.y+1];
        if (!(num&WATER || num==BORDER)) shoreLocations |= 0x04;
    }
    // check the cell to the left
    if (cell.x >= 1) {
        int num = grid[cell.x-1][cell.y];
        if (!(num&WATER || num==BORDER)) shoreLocations |= 0x08;
    }
    // check the corners
    if (!(shoreLocations&0x09)) { // no top or left shoreline
        // check cell to the top left
        if (cell.x >= 1 && cell.y >= 1) {
            int num = grid[cell.x-1][cell.y-1];
            if (!(num&WATER || num==BORDER)) shoreLocations |= 0x10;
        }
    }
    if (!(shoreLocations&0x03)) { // no top or right shoreline
        // check the cell to the top right
        if (cell.x < dimensions.x && cell.y >= 1) {
            int num = grid[cell.x+1][cell.y-1];
            if (!(num&WATER || num==BORDER)) shoreLocations |= 0x20;
        }
    }
    if (!(shoreLocations&0x06)) { // no bottom or right shoreline
        // check the cell to the bottom right
        if (cell.x < dimensions.x && cell.y < dimensions.y) {
            int num = grid[cell.x+1][cell.y+1];
            if (!(num&WATER || num==BORDER)) shoreLocations |= 0x40;
        }
    }
    if (!(shoreLocations&0x0C)) { // no bottom or left shoreline
        // check the cell to the bottom left
        if (cell.x >= 1 && cell.y < dimensions.y) {
            int num = grid[cell.x-1][cell.y+1];
            if (!(num&WATER || num==BORDER)) shoreLocations |= 0x80;
        }
    }

    // now, based off shore locations, draw the shorelines
    if (shoreLocations & 0xF0) { // there are corner shorelines
        for (int i = 0; i < 4; i++) {
            // if theere is a corner for the specified bit, draw the corresponding corner
            Uint8 bit = shoreLocations & (0x10<<i);
            // draw the image, rotated 90 degrees per corner
            if (bit) tEditor.renderTextureToTexture(BGTexture, shoreline4Tex, &cellRect, 90.0*i);
        }
    }

    // count the number of surrounding shores by counting the number of the first four bits activated
    int numShores = numBits(static_cast<int>(shoreLocations&0x0f));

    switch (numShores)
    {
        case 1: 
            // only one shoreline exists, find it, and draw it
            for (int i = 0; i < 4; i++) {
                Uint8 bit = shoreLocations & (0x01<<i);
                if (bit) tEditor.renderTextureToTexture(BGTexture, shoreline0Tex, &cellRect, 90.0*i);
            }
            break;

        case 2: {
            // if the two shores are opposite each other (top/bottom, or left/right), then shoreline0
            // will be drawn twice. if they are next to each other, shoreline1 will be drawn
            int val = shoreLocations & 0x0F; // first 4 bits

            if ((shoreLocations&0x05)==val || (shoreLocations&0x0A)==val) { // top/bottom OR left/right
                // draw shoreline 0 at any place a bit is active
                for (int i = 0; i < 4; i++) {
                    // if theere is a corner for the specified bit, draw the corresponding corner
                    Uint8 bit = shoreLocations & (0x01<<i);
                    // draw the image, rotated 90 degrees per corner
                    if (bit) tEditor.renderTextureToTexture(BGTexture, shoreline0Tex, &cellRect, 90.0*i);
                }
            } else {
                // draw shoreline1 in the appropriate location
                // check if the top AND left corners have a shore
                if ((shoreLocations&0x09)==val) tEditor.renderTextureToTexture(BGTexture, shoreline1Tex, &cellRect);
                else {
                    // check all other side combinations, in order
                    for (int i = 0; i < 3; i++) {
                        Uint8 ref = 0x03<<i, bits = shoreLocations&ref;
                        if (bits == ref) tEditor.renderTextureToTexture(BGTexture, shoreline1Tex, &cellRect, 90.0*(i+1));
                    }
                }
            }
            break;
        }
        case 3:
            // find the side that DOESN'T have a shore
            for (int i = 0; i < 4; i++) {
                Uint8 bit = shoreLocations & (0x01<<i);
                if (!bit) tEditor.renderTextureToTexture(BGTexture, shoreline2Tex, &cellRect, 90.0*i, SDL_FLIP_VERTICAL);
            }
            break;

        case 4:
            // all four sides occupied, just draw shoreline 3 to the cell
            tEditor.renderTextureToTexture(BGTexture, shoreline3Tex, &cellRect);
            break;
    
        default: break; // no shores, nothing more needs to be done
    }
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
    int nx = currLevel->gridDimensions.x, ny = currLevel->gridDimensions.y;
    map = { 0, 0, nx*sideLen, ny*sideLen };
    
    BGTexture = tEditor.createEmptyTexture(map.w, map.h, window);
    overlayTexture = tEditor.createEmptyTexture(map.w, map.h, window);

    for (int x = 0; x < nx; x++) {
        for (int y = 0; y < ny; y++) {
            PlaceObjectInCell(Vector2Int(x, y), currLevel->grid[x][y], false);
        }
    }
}