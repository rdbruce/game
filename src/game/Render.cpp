#include "Game.hpp"

bool is_in_region(Vector2 p, SDL_Rect rect) {
    int r = rect.x + rect.w, b = rect.y + rect.h;
    return p.x>=rect.x && p.x<=r && p.y>=rect.y && p.y <= b;
}

void Game::render_controls()
{
    SDL_Rect mRect = {0, 70, 24, 30};
    std::string txt;

    if (currLevel->held != nullptr) 
    {
        int hp = currLevel->held->get_hp();
        if (hp == 1) {
            // do LMB / MMB
            LMBTex->render(mRect.x, mRect.y, &mRect); mRect.x += mRect.w;
            txt = "/";
            if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                std::cerr << "couldn't render controls text!" << std::endl;
                return;
            }
            controlsTex->render(mRect.x, mRect.y); mRect.x += controlsTex->getWidth();
            MMBTex->render(mRect.x, mRect.y, &mRect); mRect.x += mRect.w;
            txt = "Throw item";
            if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                std::cerr << "couldn't render controls text!" << std::endl;
                return;
            }
            controlsTex->render(mRect.x, mRect.y); mRect.x = 0;
            mRect.y += 30;
        
        } else {
            // left click to throw one item
            txt = "Throw one item";
            if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                std::cerr << "couldn't render controls text!" << std::endl;
                return;
            }
            LMBTex->render(mRect.x, mRect.y, &mRect);
            controlsTex->render(mRect.w, mRect.y); mRect.y += 30;
            
            // middle click to throw the stack
            txt = "Throw all " + std::to_string(hp) + " items";
            if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                std::cerr << "couldn't render controls text!" << std::endl;
                return;
            }
            MMBTex->render(mRect.x, mRect.y, &mRect);
            controlsTex->render(mRect.w, mRect.y); mRect.y += 30;
        }


        int heldType = currLevel->held->get_type();
        bool render_flag = false;
        switch (heldType)
        {
            case Log_Item: 
                // right click to build log
                txt = "Build log";
                render_flag = true;
                break;

            case Pine_Cone_Item:
                // right click to plant sapling
                txt = "Plant sapling";
                render_flag = true;
                break;

            case Bridge_Item: 
                // right click to build bridge
                txt = "Build bridge";
                render_flag = true;
                break;

            case Door_Item: 
                // right click to build door
                txt = "Build door";
                render_flag = true;
                break;

            case Berry_Item:
                // right click to heal 1 hp
                txt = "Heal 1 hp";
                render_flag = true;
                break;
        }
        if (render_flag) {
            if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                std::cerr << "couldn't render controls text!" << std::endl;
                return;
            }
            RMBTex->render(mRect.x, mRect.y, &mRect);
            controlsTex->render(mRect.w, mRect.y); mRect.y += 30;
        }



    } else {
        // see if the mouse is hovering over anything
        Vector2 mPos = find_mouse_pos();
        int sideLen = currLevel->cell_sideLen;
        Vector2Int cell(mPos.x/sideLen, mPos.y/sideLen);
        int type = currLevel->grid[cell.x][cell.y]&255;

        if (type) {
            bool render_flag = false;
            switch (type) 
            {
                case 7: // closed door
                    txt = "Open";
                    render_flag = true;
                    break;

                case 8: // open door
                    txt = "Close";
                    render_flag = true;
                    break;
            }
            if (render_flag) {
                if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                    std::cerr << "couldn't render controls text!" << std::endl;
                    return;
                }
                LMBTex->render(mRect.x, mRect.y, &mRect);
                controlsTex->render(mRect.w, mRect.y); mRect.y += 30;
            }

            if (is_occupied(cell)) {
                if (!(type&INDESTRUBTIBLE)) {
                    RMBTex->render(mRect.x, mRect.y, &mRect);
                    txt = ((type) == 10)? "Pick" : "Destroy";
                    if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                        std::cerr << "couldn't render controls text!" << std::endl;
                        return;
                    }
                    controlsTex->render(mRect.w, mRect.y); mRect.y += 30;
                }
            }
        } else {
            // not hovering over any cell, check the game objects
            int n = currLevel->gameObjects.size();
            for (int i = 0; i < n; i++) 
            {
                auto obj = currLevel->gameObjects[i];
                SDL_Rect hitbox = obj->get_hitbox();
                if (is_in_region(mPos, hitbox)) {
                    EntityType type = obj->get_type();
                    switch (type)
                    {
                        case Log_Item: {
                            LMBTex->render(mRect.x, mRect.y, &mRect);
                            txt = "Pick up item";
                            if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                                std::cerr << "couldn't render controls text!" << std::endl;
                                return;
                            }
                            controlsTex->render(mRect.w, mRect.y); mRect.y += 30;

                            RMBTex->render(mRect.x, mRect.y, &mRect);
                            txt = "Craft 2 ";
                            if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                                std::cerr << "couldn't render controls text!" << std::endl;
                                return;
                            }
                            controlsTex->render(mRect.w, mRect.y); mRect.x += controlsTex->getWidth() + mRect.w;

                            SDL_Rect itemRect = {mRect.x, mRect.y, 30, 30};
                            plankTex->render(mRect.x, mRect.y, &itemRect); mRect.x += itemRect.w;
                            txt = "(1 ";
                            if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                                std::cerr << "couldn't render controls text!" << std::endl;
                                return;
                            }
                            controlsTex->render(mRect.x, mRect.y); mRect.x += controlsTex->getWidth();

                            logTex->render(mRect.x, mRect.y, &itemRect); mRect.x += itemRect.w;
                            txt = ")";
                            if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                                std::cerr << "couldn't render controls text!" << std::endl;
                                return;
                            }
                            controlsTex->render(mRect.x, mRect.y); mRect.x = 0; mRect.y += 30;
                            break;
                        }

                        case Plank_Item: {
                            LMBTex->render(mRect.x, mRect.y, &mRect);
                            txt = "Pick up item";
                            if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                                std::cerr << "couldn't render controls text!" << std::endl;
                                return;
                            }
                            controlsTex->render(mRect.w, mRect.y); mRect.y += 30;

                            if (obj->get_hp() >= 4) 
                            {
                                RMBTex->render(mRect.x, mRect.y, &mRect);
                                txt = "Craft 1 ";
                                if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                                    std::cerr << "couldn't render controls text!" << std::endl;
                                    return;
                                }
                                controlsTex->render(mRect.w, mRect.y); mRect.x += controlsTex->getWidth() + mRect.w;

                                SDL_Rect itemRect = {mRect.x, mRect.y, 30, 30};
                                closed_doorTex->render(mRect.x, mRect.y, &itemRect); mRect.x += itemRect.w;
                                txt = "(4 ";
                                if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                                    std::cerr << "couldn't render controls text!" << std::endl;
                                    return;
                                }
                                controlsTex->render(mRect.x, mRect.y); mRect.x += controlsTex->getWidth();

                                plankTex->render(mRect.x, mRect.y, &itemRect); mRect.x += itemRect.w;
                                txt = ")";
                                if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                                    std::cerr << "couldn't render controls text!" << std::endl;
                                    return;
                                }
                                controlsTex->render(mRect.x, mRect.y); mRect.x = 0; mRect.y += 30;
                            }
                            break;
                        }

                        case Bridge_Item: {
                            LMBTex->render(mRect.x, mRect.y, &mRect);
                            txt = "Pick up item";
                            if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                                std::cerr << "couldn't render controls text!" << std::endl;
                                return;
                            }
                            controlsTex->render(mRect.w, mRect.y); mRect.y += 30;

                            RMBTex->render(mRect.x, mRect.y, &mRect);
                            txt = "Craft 2 ";
                            if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                                std::cerr << "couldn't render controls text!" << std::endl;
                                return;
                            }
                            controlsTex->render(mRect.w, mRect.y); mRect.x += controlsTex->getWidth() + mRect.w;

                            SDL_Rect itemRect = {mRect.x, mRect.y, 30, 30};
                            logTex->render(mRect.x, mRect.y, &itemRect); mRect.x += itemRect.w;

                            txt = ", 4";
                            if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                                std::cerr << "couldn't render controls text!" << std::endl;
                                return;
                            }
                            controlsTex->render(mRect.x, mRect.y); mRect.x += controlsTex->getWidth();
                            
                            plankTex->render(mRect.x, mRect.y, &itemRect); mRect.x += itemRect.w;

                            txt = "(1 ";
                            if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                                std::cerr << "couldn't render controls text!" << std::endl;
                                return;
                            }
                            controlsTex->render(mRect.x, mRect.y); mRect.x += controlsTex->getWidth();

                            bridgeTex->render(mRect.x, mRect.y, &itemRect); mRect.x += itemRect.w;
                            txt = ")";
                            if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                                std::cerr << "couldn't render controls text!" << std::endl;
                                return;
                            }
                            controlsTex->render(mRect.x, mRect.y); mRect.x = 0; mRect.y += 30;
                            break;
                        }

                        case Door_Item: {
                            LMBTex->render(mRect.x, mRect.y, &mRect);
                            txt = "Pick up item";
                            if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                                std::cerr << "couldn't render controls text!" << std::endl;
                                return;
                            }
                            controlsTex->render(mRect.w, mRect.y); mRect.y += 30;

                            RMBTex->render(mRect.x, mRect.y, &mRect);
                            txt = "Craft 4 ";
                            if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                                std::cerr << "couldn't render controls text!" << std::endl;
                                return;
                            }
                            controlsTex->render(mRect.w, mRect.y); mRect.x += controlsTex->getWidth() + mRect.w;

                            SDL_Rect itemRect = {mRect.x, mRect.y, 30, 30};
                            plankTex->render(mRect.x, mRect.y, &itemRect); mRect.x += itemRect.w;
                            txt = "(1 ";
                            if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                                std::cerr << "couldn't render controls text!" << std::endl;
                                return;
                            }
                            controlsTex->render(mRect.x, mRect.y); mRect.x += controlsTex->getWidth();

                            closed_doorTex->render(mRect.x, mRect.y, &itemRect); mRect.x += itemRect.w;
                            txt = ")";
                            if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                                std::cerr << "couldn't render controls text!" << std::endl;
                                return;
                            }
                            controlsTex->render(mRect.x, mRect.y); mRect.x = 0; mRect.y += 30;
                            break;
                        }

                        default:
                            if (obj->is_item()) {
                                LMBTex->render(mRect.x, mRect.y, &mRect);
                                txt = "Pick up item";
                                if (!controlsTex->loadFromRenderedText(txt, {255,255,255,255})) {
                                    std::cerr << "couldn't render controls text!" << std::endl;
                                    return;
                                }
                                controlsTex->render(mRect.w, mRect.y); mRect.y += 30;
                            }
                            break;
                    }
                }
            }
        }
    }
}







void Game::center_camera_on_player()
{
    Vector2 pPos = currLevel->player->get_pos(),
            cPos = pPos - Vector2(camera.w/2, camera.h/2);
    
    // if the camera is on the edge of the map, snap it back in bounds
    cPos.x = clampf(0.0f, map.w-camera.w, cPos.x);
    cPos.y = clampf(0.0f, map.h-camera.h, cPos.y);

    camera.x = cPos.x; camera.y = cPos.y;
}

void Game::render_framerate()
{
    int newFPS = 1.0f / deltaTime;
    fps = (newFPS == Clamp(0, 5000, newFPS))? newFPS : fps;
    std::string txt = std::to_string(fps) + "FPS";
    if (!fpsTex->loadFromRenderedText(txt, {255,255,255,255})) {
        std::cerr << "couldn't render FPS!" << std::endl;
        return;
    }
    fpsTex->render(0, 0);
}

void Game::render_player_health()
{
    SDL_Rect heartRect = {0, 30, 50, 50};
    int full = currLevel->player->get_hp();

    for (int i = 0; i < 5; i++) 
    {
        if (i < full) full_heartTex->render(heartRect.x, heartRect.y, &heartRect);
        else empty_heartTex->render(heartRect.x, heartRect.y, &heartRect);
        heartRect.x += heartRect.w;
    }
}

// renders the scene's background
void Game::render_background() {
    BGTexture->render(0, 0, &camera, &camera);
}

void Game::render_overlay() {
    overlayTexture->render(0, 0, &camera, &camera);
    CRT->renderAsBackground();
}



// renders all the game objects
void Game::render_gameobjects() {
    for (int i = 0; i < currLevel->gameObjects.size(); i++) {
        if (switching_scenes) {
            switching_scenes = false; return;
        }
        currLevel->gameObjects[i]->render( camera.x, camera.y );
    }
}