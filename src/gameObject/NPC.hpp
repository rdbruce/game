#pragma once

#include "GameObject.hpp"

class NPC : public GameObject
{
    public:

        NPC(int type, Vector2 pos, int Idx, Game *game, int cell_sidelen);

        virtual void update();
        virtual void render(int camX, int camY, Uint8 alpha = 255);

        void set_dialogueTimer(float newTimer);

    private:

        std::shared_ptr<LTexture> tex;

        float moveSpeed;

        /* for updating position */
        void (NPC::*updatePos)();
        void foxPosition();
        void rabbitPosition();
        void bearPosition();

        /* for updating velocity */
        void (NPC::*updateVel)();
        void foxVelocity();

        /* for exiting the scene */
        void beginRetreat();
        void retreat();


        /* for showing dialogue */
        void (NPC::*dialogue)();
        void foxDialogue();
        void bearDialogue();
        void rabbitDialogue();

        float dialogueInterval = 0.25f, dialogueTimer = 0.0f;


        /* collision */
        void collision();
        void handleCollisionWithGameObjects();
        void collideWithWorldBorders();
        void handleCornerCollisionsWithWalls();



};