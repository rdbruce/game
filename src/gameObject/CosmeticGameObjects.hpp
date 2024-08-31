#pragma once

#include "GameObject.hpp"

class FallingTree : public GameObject
{
    public:

        FallingTree(Vector2 pos, int Idx, Game *game, int cell_sidelen);

        virtual void update();

        virtual void render(int camX, int camY, Uint8 alpha = 255);

    private:

        std::shared_ptr<LTexture> tex;
        
        float timer = 1.0f;
};