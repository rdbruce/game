#include "CosmeticGameObjects.hpp"
#include "../game/Game.hpp"

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