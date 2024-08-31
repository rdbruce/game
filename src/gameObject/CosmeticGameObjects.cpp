#include "CosmeticGameObjects.hpp"
#include "../game/Game.hpp"

/*          FALLING TREES          */

FallingTree::FallingTree(Vector2 pos, int Idx, Game* game, int cellSideLen)
:   GameObject(fallingTree, pos, Idx, 1, game, cellSideLen, false, 3*cellSideLen, 10*cellSideLen),
    tex(game->falling_treeTex) {}

void FallingTree::render(int camX, int camY, Uint8 alpha)
{
    SDL_Rect hitbox = get_hitbox();
    Vector2Int p( hitbox.x - camX, hitbox.y-camY );
    // not within the camera's view, don't render
    if (p.x != Clamp(game->renderOffset.x-hitbox.x, game->camera.w+game->renderOffset.x, p.x) || p.y != Clamp(game->renderOffset.y-hitbox.h, game->camera.h+game->renderOffset.y, p.y)) {
        return;
    }
    // timer is initialised to 1.0f, use it as an interpolator
    float interp = 1.0f - timer;
    float theta = -100.0f * interp;
    SDL_Point centre = { hitbox.w/2, hitbox.h };
    tex->render(p.x, p.y, &hitbox, NULL, theta, &centre);
}

void FallingTree::update()
{
    if (timer <= 0.0f)
    {
        // once the tree is gone, spawn some log + pine cone items in where it fell
        // the further log will be the height of the tree (size.x) to the LEFT of pos,
        // the nearest one could be at pos

        // 5 wood will spawn, 0-2 seeds will spawn
        int seeds = rand()%3, n = seeds+5;

        SDL_Rect hitbox = get_hitbox();
        Vector2Int size(hitbox.w/2, hitbox.h/2);
        int sideLen = get_cellSidelen();
        
        float step = (sideLen*10)/n, y = hitbox.y + hitbox.h;

        // spawn n items
        for (int i = 1; i <= n; i++)
        {
            Vector2 p(hitbox.x - step*i, y);
            // spawn logs first, seeds last
            EntityType item = (i<6)? logItem : pineConeItem;
            game->spawnItemStack(item, p, 1);
        }
        Destroy();
    }
    timer -= get_deltaTime();
}




/*          GHOST BUILDINGS          */
GhostBuilding::GhostBuilding(Vector2Int cell, EntityType itemType, int Idx, Game *game, int cell_sideLen)
: GameObject(ghostBuilding, Vector2(((float)cell.x+0.5f)*cell_sideLen,((float)cell.x+0.5f)*cell_sideLen), Idx, 1, game, cell_sideLen, false, cell_sideLen)
{
    switch (itemType)
    {
        case logItem: tex = game->logTex; break;
        case pineConeItem: tex = game->saplingTex; break;
        case damItem: tex = game->damTex; break;
        case doorItem: tex = game->closed_doorTex; break;
    }
}

void GhostBuilding::update()
{
    (this->*Update)();
}

void GhostBuilding::validate()
{
    if (tex == nullptr) Destroy();
    else Update = &GhostBuilding::go_to_mouse_cell;
}

void GhostBuilding::go_to_mouse_cell()
{
    // ensure that there is still something being held
    if (game->currLevel->held == nullptr) Destroy();

    Vector2 mPos = game->find_mouse_pos();
    float sideLen = get_cellSidelen();
    mPos /= sideLen;
    mPos.x = (int)mPos.x; mPos.y = (int)mPos.y;
    mPos *= sideLen;
    mPos += Vector2_One * 0.5f * sideLen;

    set_pos(mPos);
}

void GhostBuilding::render(int camX, int camY, Uint8 alpha)
{
    if (alpha == 255) {
        game->secondRenders.push(this);
    } else {
        SDL_Rect hitbox = get_hitbox();
        Vector2Int p( hitbox.x - camX, hitbox.y-camY );
        // not within the camera's view, don't render
        if (p.x != Clamp(game->renderOffset.x-hitbox.x, game->camera.w+game->renderOffset.x, p.x) || p.y != Clamp(game->renderOffset.y-hitbox.h, game->camera.h+game->renderOffset.y, p.y)) {
            return;
        }
        tex->setAlpha(alpha);
        tex->render(p.x, p.y, &hitbox);
        tex->setAlpha(255);
    }
}