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

class GhostBuilding : public GameObject
{
    public :

        GhostBuilding(Vector2Int cell, EntityType itemType, int Idx, Game *game, int cell_sideLen);

        virtual void update();

        virtual void render(int camX, int camY, Uint8 alpha = 255);

    private:

        void (GhostBuilding::*Update)() = &GhostBuilding::validate;
        void validate();
        void go_to_mouse_cell();

        std::shared_ptr<LTexture> tex = nullptr;
};