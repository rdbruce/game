#include "../gameObject/GameObject.hpp"
#include "Game.hpp"
#include "Dialogue.hpp"

void GameObject::foxRenderFunc( int camX, int camY, Uint8 alpha )
{
    Vector2Int p( hitbox.x-camX, hitbox.y-camY );
    // not within the camera's view, don't render
    if (p.x != Clamp(game->renderOffset.x-hitbox.x, game->camera.w+game->renderOffset.x, p.x) || p.y != Clamp(game->renderOffset.y-hitbox.h, game->camera.h+game->renderOffset.y, p.y)) {
        return;
    }
    tex->render( p.x, p.y, &hitbox );


    // exit dialogue if the player walks too far away
    Vector2 pPos = get_player_pos();
    float dist = (pPos - pos).length();
    if (dist > 1.5f*game->interactRange) {
        if (game->currDialogue > FOX_DIAG_MIN && game->currDialogue < FOX_DIAG_MAX) {
            game->enter_dialogue(None);
        }
        hp = 1; return;
    }

    Vector2Int cell = get_cell();
    if (game->is_under_tree(cell) && alpha == 255) {
        game->secondRenders.push(this);
    }
    else 
    {
        if (game->currLevel == &game->Base) 
        {
            switch (hp)
            {
                case 2: 
                {
                    if (game->currDialogue == fox_base_dialogue_1_1) {
                        hp = 4; timer = 0.25f; break;
                    } else if (game->currDialogue == fox_base_dialogue_1_2) {
                        hp = 6; timer = 0.25f; break;
                    }

                    std::string rend, txt = "Hey, what are you doing here?";
                    if (timer >= 0.0f) {
                        float t = 1.0f - (timer/0.25f);
                        int n = Max(1, t * txt.size());
                        rend = txt.substr(0, n);
                        timer -= get_deltaTime();
                    } else {
                        rend = txt;
                        game->enter_dialogue(fox_base_dialogue_1);
                    }

                    auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x-camX, p.y-30), game->window);
                    game->dialogueRenders.push(diag);
                    break;
                }

                case 3: hp = 2; timer = 0.25f; break; // go back

                case 4:
                {
                    if (game->currDialogue == fox_base_dialogue_2_1) {
                        hp = 8; timer = 0.25f; break;
                    }

                    std::string rend, txt = "Good question. Nowhere,\nreally. You ought to go\nnorth of here";
                    if (timer >= 0.0f) {
                        float t = 1.0f - (timer/0.25f);
                        int n = Max(1, t * txt.size());
                        rend = txt.substr(0, n);
                        timer -= get_deltaTime();
                    } else {
                        rend = txt;
                        game->enter_dialogue(fox_base_dialogue_2);
                    }

                    auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x-camX, p.y-90), game->window);
                    game->dialogueRenders.push(diag);
                    break;
                }

                case 5: hp = 4; timer = 0.25f; break; // go back

                case 6:
                {
                    std::string rend, txt = "What's it to you?";
                    if (timer >= 0.0f) {
                        float t = 1.0f - (timer/0.25f);
                        int n = Max(1, t * txt.size());
                        rend = txt.substr(0, n);
                        timer -= get_deltaTime();
                    } else {
                        rend = txt;
                    }

                    auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x-camX, p.y-30), game->window);
                    game->dialogueRenders.push(diag);
                    break;
                }

                case 7:
                    game->enter_dialogue(None);
                    hp = 2; timer = 0.25f;
                    break;

                case 8:
                {
                    if (game->currDialogue == fox_base_dialogue_3_1) {
                        hp = 10; timer = 0.25f; break;
                    } else if (game->currDialogue == fox_base_dialogue_3_2) {
                        hp = 13; timer = 0.25f; break;
                    }

                    std::string rend, txt = "How am I supposed to know?\nDo whatever beavers do,\nbuild a dam or something";
                    if (timer >= 0.0f) {
                        float t = 1.0f - (timer/0.25f);
                        int n = Max(1, t * txt.size());
                        rend = txt.substr(0, n);
                        timer -= get_deltaTime();
                    } else {
                        rend = txt;
                        game->enter_dialogue(fox_base_dialogue_3);
                    }

                    auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x-camX, p.y-90), game->window);
                    game->dialogueRenders.push(diag);
                    break;
                }

                case 9: hp = 8; timer = 0.25f; break; // go back

                case 10:
                {
                    std::string rend, txt = "Some beaver you are...";
                    if (timer >= 0.0f) {
                        float t = 1.0f - (timer/0.25f);
                        int n = Max(1, t * txt.size());
                        rend = txt.substr(0, n);
                        timer -= get_deltaTime();
                    } else {
                        rend = txt;
                    }

                    auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x-camX, p.y-30), game->window);
                    game->dialogueRenders.push(diag);
                    break;
                }

                case 11:
                {
                    std::string rend, txt = "Look just put\n \nand figure it out";
                    if (timer >= 0.0f) {
                        float t = 1.0f - (timer/0.25f);
                        int n = Max(1, t * txt.size());
                        rend = txt.substr(0, n);
                        timer -= get_deltaTime();
                    } else {
                        rend = txt;
                    }

                    std::vector<std::string> vec = {rend, "1 ", " and 4 ", " together"};
                    SDL_Color colour = {255,255,255,255};

                    auto diag = std::make_shared<DialogueRender>(
                        vec, Vector2Int(pos.x-camX, p.y-60), game->window, colour,
                        Centred, &DialogueRender::fox_base_case11, 
                        game->logTex, game->plankTex
                    );
                    game->dialogueRenders.push(diag);

                    break;
                }

                case 12:
                    beginRetreat();
                    game->enter_dialogue(None);
                    hp = 1;
                    break;

                case 13:
                {
                    std::string rend, txt = "Don't worry about that";
                    if (timer >= 0.0f) {
                        float t = 1.0f - (timer/0.25f);
                        int n = Max(1, t * txt.size());
                        rend = txt.substr(0, n);
                        timer -= get_deltaTime();
                    } else {
                        rend = txt;
                    }

                    auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x-camX, p.y-30), game->window);
                    game->dialogueRenders.push(diag);
                    break;
                }

                case 14:
                    hp = 11; timer = 0.25f;
                    game->enter_dialogue(fox_base_dialogue_3_1);
                    break;
            }
        }
        else
        {
            switch (hp)
            {
                case 2:
                {
                    if (game->currDialogue == fox_town_1_1) {
                        hp = 4; timer = 0.25f; break;
                    } else if (game->currDialogue == fox_town_1_2) {
                        hp = 7; timer = 0.25f; break;
                    } else if (game->currDialogue == fox_town_1_3) {
                        hp = 9; timer = 0.25f; break;
                    }

                    std::string rend, txt = "Heyy you made it up here";
                    if (timer >= 0.0f) {
                        float t = 1.0f - (timer/0.25f);
                        int n = Max(1, t * txt.size());
                        rend = txt.substr(0, n);
                        timer -= get_deltaTime();
                    } else {
                        rend = txt;
                        game->enter_dialogue(fox_town_1);
                    }

                    auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x-camX, p.y-30), game->window);
                    game->dialogueRenders.push(diag);
                    break;
                }

                case 3: hp = 2; timer = 0.25f; break; // go back

                case 4:
                {
                    std::string rend, txt = "You ask too many questions";
                    if (timer >= 0.0f) {
                        float t = 1.0f - (timer/0.25f);
                        int n = Max(1, t * txt.size());
                        rend = txt.substr(0, n);
                        timer -= get_deltaTime();
                    } else {
                        rend = txt;
                    }

                    auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x-camX, p.y-30), game->window);
                    game->dialogueRenders.push(diag);
                    break;
                }

                case 5:
                {
                    std::string rend, txt = "Look just ask around, you\ncan buy stuff, nothing\n too crazy";
                    if (timer >= 0.0f) {
                        float t = 1.0f - (timer/0.25f);
                        int n = Max(1, t * txt.size());
                        rend = txt.substr(0, n);
                        timer -= get_deltaTime();
                    } else {
                        rend = txt;
                    }

                    auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x-camX, p.y-90), game->window);
                    game->dialogueRenders.push(diag);
                    break;
                }

                case 6:
                    hp = 1;
                    game->enter_dialogue(None);
                    break;

                case 7:
                {
                    std::string rend, txt = "Don't. Worry about that.";
                    if (timer >= 0.0f) {
                        float t = 1.0f - (timer/0.25f);
                        int n = Max(1, t * txt.size());
                        rend = txt.substr(0, n);
                        timer -= get_deltaTime();
                    } else {
                        rend = txt;
                    }

                    auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x-camX, p.y-30), game->window);
                    game->dialogueRenders.push(diag);
                    break;
                }

                case 8:
                    hp = 5; timer = 0.25f;
                    break;

                case 9:
                {
                    std::string rend, txt = "Oh, those guys...";
                    if (timer >= 0.0f) {
                        float t = 1.0f - (timer/0.25f);
                        int n = Max(1, t * txt.size());
                        rend = txt.substr(0, n);
                        timer -= get_deltaTime();
                    } else {
                        rend = txt;
                    }

                    auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x-camX, p.y-30), game->window);
                    game->dialogueRenders.push(diag);
                    break;
                }

                case 10:
                {
                    if (game->currDialogue == fox_town_2_1) {
                        hp = 12; timer = 0.25f; break;
                    } else if (game->currDialogue == fox_town_2_2) {
                        hp = 14; timer = 0.25f; break;
                    } else if (game->currDialogue == fox_town_2_3) {
                        hp = 17; timer = 0.25f; break;
                    }

                    std::string rend, txt = "Yeah sorry about them, just do\nyour best to survive, I guess";
                    if (timer >= 0.0f) {
                        float t = 1.0f - (timer/0.25f);
                        int n = Max(1, t * txt.size());
                        rend = txt.substr(0, n);
                        timer -= get_deltaTime();
                    } else {
                        rend = txt;
                        game->enter_dialogue(fox_town_2);
                    }

                    auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x-camX, p.y-60), game->window);
                    game->dialogueRenders.push(diag);
                    break;
                }

                case 11: hp = 10; timer = 0.25f; break; // go back

                case 12:
                {
                    std::string rend, txt = "Oops...";
                    if (timer >= 0.0f) {
                        float t = 1.0f - (timer/0.25f);
                        int n = Max(1, t * txt.size());
                        rend = txt.substr(0, n);
                        timer -= get_deltaTime();
                    } else {
                        rend = txt;
                    }

                    auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x-camX, p.y-30), game->window);
                    game->dialogueRenders.push(diag);
                    break;
                }

                case 13: 
                    game->enter_dialogue(fox_town_2);
                    hp = 10; timer = 0.25f; 
                    break;

                case 14:
                {
                    std::string rend, txt = "I mean you've made it so far...";
                    if (timer >= 0.0f) {
                        float t = 1.0f - (timer/0.25f);
                        int n = Max(1, t * txt.size());
                        rend = txt.substr(0, n);
                        timer -= get_deltaTime();
                    } else {
                        rend = txt;
                    }

                    auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x-camX, p.y-30), game->window);
                    game->dialogueRenders.push(diag);
                    break;
                }

                case 15:
                {
                    std::string rend, txt = "Just build walls, throw rocks, etc.";
                    if (timer >= 0.0f) {
                        float t = 1.0f - (timer/0.25f);
                        int n = Max(1, t * txt.size());
                        rend = txt.substr(0, n);
                        timer -= get_deltaTime();
                    } else {
                        rend = txt;
                    }

                    auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x-camX, p.y-30), game->window);
                    game->dialogueRenders.push(diag);
                    break;
                }

                case 16: 
                    game->enter_dialogue(fox_town_2);
                    hp = 10; timer = 0.25f; 
                    break;

                case 17:
                {
                    std::string rend, txt = "Yeahh... this is a..\n\"no soliciting\" sorta thing...";
                    if (timer >= 0.0f) {
                        float t = 1.0f - (timer/0.25f);
                        int n = Max(1, t * txt.size());
                        rend = txt.substr(0, n);
                        timer -= get_deltaTime();
                    } else {
                        rend = txt;
                    }

                    auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x-camX, p.y-60), game->window);
                    game->dialogueRenders.push(diag);
                    break;
                }

                case 18:
                    hp = 1;
                    game->enter_dialogue(None);
                    break;
            }
        }
    }
}

void GameObject::bearRenderFunc( int camX, int camY, Uint8 alpha )
{
    Vector2Int p( hitbox.x-camX, hitbox.y-camY );
    // not within the camera's view, don't render
    if (p.x != Clamp(game->renderOffset.x-hitbox.x, game->camera.w+game->renderOffset.x, p.x) || p.y != Clamp(game->renderOffset.y-hitbox.h, game->camera.h+game->renderOffset.y, p.y)) {
        return;
    }
    tex->render( p.x, p.y, &hitbox );

    // exit dialogue if the player walks too far away
    Vector2 pPos = get_player_pos();
    float dist = (pPos - pos).length();
    if (dist > 1.5f*game->interactRange) {
        if (game->currDialogue > BEAR_DIAG_MIN && game->currDialogue < BEAR_DIAG_MAX) {
            game->enter_dialogue(None);
        }
        hp = 1; return;
    }

    switch(hp)
    {
        case 2:
        {
            if (game->currDialogue == bear_town_1_1) {
                hp = 4; timer = 0.25f; break;
            } else if (game->currDialogue == bear_town_1_2) {
                hp = 6; timer = 0.25f; break;
            }

            std::string rend, txt = "Hello :)";
            if (timer >= 0.0f) {
                float t = 1.0f - (timer/0.25f);
                int n = Max(1, t * txt.size());
                rend = txt.substr(0, n);
                timer -= get_deltaTime();
            } else {
                rend = txt;
                game->enter_dialogue(bear_town_1);
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x - camX, p.y), game->window);
            game->dialogueRenders.push(diag);
            break;
        }

        case 3: hp = 2; timer = 0.25f; break;

        case 4:
        {
            if (game->currDialogue == bear_town_2_1) {
                hp = 8; timer = 0.25f; break;
            } else if (game->currDialogue == bear_town_2_2) {
                hp = 10; timer = 0.25f; break;
            }

            std::string rend, txt = "Do you like berries?";
            if (timer >= 0.0f) {
                float t = 1.0f - (timer/0.25f);
                int n = Max(1, t * txt.size());
                rend = txt.substr(0, n);
                timer -= get_deltaTime();
            } else {
                rend = txt;
                game->enter_dialogue(bear_town_2);
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x - camX, p.y), game->window);
            game->dialogueRenders.push(diag);
            break;
        }

        case 5: hp = 4; timer = 0.25f; break;

        case 6:
        {
            if (game->currDialogue == bear_town_3_1) {
                hp = 8; timer = 0.25f; break;
            } else if (game->currDialogue == bear_town_3_2) {
                hp = 12; timer = 0.25f; break;
            }

            std::string rend, txt = "Your mom ;)";
            if (timer >= 0.0f) {
                float t = 1.0f - (timer/0.25f);
                int n = Max(1, t * txt.size());
                rend = txt.substr(0, n);
                timer -= get_deltaTime();
            } else {
                rend = "Eat berries :)";
                game->enter_dialogue(bear_town_3);
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x - camX, p.y), game->window);
            game->dialogueRenders.push(diag);
            break;
        }

        case 7: hp = 6; timer = 0.25f; break;

        case 8:
        {
            if (game->currDialogue == bear_town_4_1) {
                hp = 14; timer = 0.25f; break;
            } else if (game->currDialogue == bear_town_4_2) {
                hp = 16; timer = 0.25f; break;
            }

            std::string rend, txt = "You want some? :D";
            if (timer >= 0.0f) {
                float t = 1.0f - (timer/0.25f);
                int n = Max(1, t * txt.size());
                rend = txt.substr(0, n);
                timer -= get_deltaTime();
            } else {
                rend = txt;
                game->enter_dialogue(bear_town_4);
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x - camX, p.y), game->window);
            game->dialogueRenders.push(diag);
            break;
        }

        case 9: hp = 8; timer = 0.25f; break;

        case 10:
        {
            std::string rend, txt = "... >:(";
            if (timer >= 0.0f) {
                float t = 1.0f - (timer/0.25f);
                int n = Max(1, t * txt.size());
                rend = txt.substr(0, n);
                timer -= get_deltaTime();
            } else {
                rend = txt;
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x - camX, p.y), game->window);
            game->dialogueRenders.push(diag);
            break;
        }

        case 11:
            hp = 1;
            game->enter_dialogue(None);
            break;
        
        case 12:
        {
            std::string rend, txt = "Eat berries :)";
            if (timer >= 0.0f) {
                float t = 1.0f - (timer/0.25f);
                int n = Max(1, t * txt.size());
                rend = txt.substr(0, n);
                timer -= get_deltaTime();
            } else {
                rend = txt;
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x - camX, p.y), game->window);
            game->dialogueRenders.push(diag);
            break;
        }

        case 13: hp = 8; timer = 0.25f; break;

        case 14:
        {
            if (game->currDialogue == bear_town_5_1) {
                hp = 1; game->enter_dialogue(None); break;
            } else if (game->currDialogue == bear_town_5_2) {
                hp = 18; timer = 0.25f; break;
            }

            game->enter_dialogue(bear_town_5);

            std::vector<std::string> vec = {"Give me 2 ", " for "};
            SDL_Color colour = {255,255,255,255};

            auto diag = std::make_shared<DialogueRender>(
                vec, Vector2Int(pos.x-camX, p.y), game->window, colour,
                Centred, &DialogueRender::bear_town_case14,
                game->stoneTex, game->berryTex
            );
            game->dialogueRenders.push(diag);

            break;
        }

        case 15: hp = 14; timer = 0.25f; break;

        case 16:
        {
            std::string rend, txt = ":P";
            if (timer >= 0.0f) {
                float t = 1.0f - (timer/0.25f);
                int n = Max(1, t * txt.size());
                rend = txt.substr(0, n);
                timer -= get_deltaTime();
            } else {
                rend = txt;
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x - camX, p.y), game->window);
            game->dialogueRenders.push(diag);
            break;
        }

        case 17:
            hp = 1;
            game->enter_dialogue(None);
            break;

        case 18:
        {
            std::string rend, txt = "Mash berry into jam :)";
            if (timer >= 0.0f) {
                float t = 1.0f - (timer/0.25f);
                int n = Max(1, t * txt.size());
                rend = txt.substr(0, n);
                timer -= get_deltaTime();
            } else {
                rend = txt;
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(pos.x - camX, p.y), game->window);
            game->dialogueRenders.push(diag);
            break;
        }

        case 19:
            hp = 1;
            game->enter_dialogue(None);
            break;
    }
}


void GameObject::playerRenderFunc( int camX, int camY, Uint8 alpha )
{
    Vector2Int p( hitbox.x-camX, hitbox.y-camY );
    // not within the camera's view, don't render
    if (p.x != Clamp(game->renderOffset.x-hitbox.x, game->camera.w+game->renderOffset.x, p.x) || p.y != Clamp(game->renderOffset.y-hitbox.h, game->camera.h+game->renderOffset.y, p.y)) {
        return;
    }
    tex = animatePlayer();

    tex->setAlpha(alpha);
    tex->render(p.x, p.y, &hitbox);
    tex->setAlpha(255); 

    Vector2Int cell = get_cell();
    if (game->is_under_tree(cell) && alpha == 255) {
        game->secondRenders.push(this);
    } 
    else 
    {
        switch (game->currDialogue)
        {
            case fox_base_dialogue_1: 
            {
                std::string rend, txt = "1. Where is here?\n2. What are YOU doing here?";

                if (timer >= 0.0f) {
                    float t = 1.0f - (timer/1.1f);
                    int n = Max(1, t * txt.size());
                    rend = txt.substr(0, n);
                    timer -= get_deltaTime();
                } else {
                    rend = txt;
                }

                auto diag = std::make_shared<DialogueRender>(
                    rend, Vector2Int(pos.x - camX, p.y - 60),
                    game->window
                );
                game->dialogueRenders.push(diag);

                break;
            }

            case fox_base_dialogue_1_1: 
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::string txt = "1. Where is here?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(pos.x - camX, p.y - 30),
                            game->window
                        );
                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case fox_base_dialogue_1_2:
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::string txt = "2. What are YOU doing here?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(pos.x - camX, p.y - 30),
                            game->window
                        );
                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case fox_base_dialogue_2:
            {
                std::string rend, txt = "1. But how do I\nget past that river?";

                if (timer >= 0.0f) {
                    float t = 1.0f - (timer/1.1f);
                    int n = Max(1, t * txt.size());
                    rend = txt.substr(0, n);
                    timer -= get_deltaTime();
                } else {
                    rend = txt;
                }

                auto diag = std::make_shared<DialogueRender>(
                    rend, Vector2Int(pos.x - camX, p.y - 60),
                    game->window
                );
                game->dialogueRenders.push(diag);

                break;
            }

            case fox_base_dialogue_2_1:
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::string txt = "1. But how do I\nget past that river?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(pos.x - camX, p.y - 60),
                            game->window
                        );
                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case fox_base_dialogue_3:
            {
                std::string rend, txt = "1. How do I do THAT?\n2. How are YOU getting there?";

                if (timer >= 0.0f) {
                    float t = 1.0f - (timer/1.1f);
                    int n = Max(1, t * txt.size());
                    rend = txt.substr(0, n);
                    timer -= get_deltaTime();
                } else {
                    rend = txt;
                }

                auto diag = std::make_shared<DialogueRender>(
                    rend, Vector2Int(pos.x - camX, p.y - 60),
                    game->window
                );
                game->dialogueRenders.push(diag);

                break;
            }

            case fox_base_dialogue_3_1:
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::string txt = "1. How do I do THAT?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(pos.x - camX, p.y - 30),
                            game->window
                        );
                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case fox_base_dialogue_3_2:
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::string txt = "2. How are YOU getting there?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(pos.x - camX, p.y - 30),
                            game->window
                        );
                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case fox_town_1:
            {
                std::string rend, txt = "1. What is this place?\n2. How did YOU make it up here?\n3. Who were all those wolves?";

                if (timer >= 0.0f) {
                    float t = 1.0f - (timer/1.1f);
                    int n = Max(1, t * txt.size());
                    rend = txt.substr(0, n);
                    timer -= get_deltaTime();
                } else {
                    rend = txt;
                }

                auto diag = std::make_shared<DialogueRender>(
                    rend, Vector2Int(pos.x - camX, p.y - 90),
                    game->window
                );
                game->dialogueRenders.push(diag);

                break;
            }

            case fox_town_1_1:
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::string txt = "1. What is this place?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(pos.x - camX, p.y - 30),
                            game->window
                        );
                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case fox_town_1_2:
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::string txt = "2. How did YOU make it up here?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(pos.x - camX, p.y - 30),
                            game->window
                        );
                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }
            
            case fox_town_1_3:
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::string txt = "3. Who were all those wolves?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(pos.x - camX, p.y - 30),
                            game->window
                        );
                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case fox_town_2:
            {
                std::string rend, txt = "1. You could've told me that before!\n2. Do you have any advice?\n3. Can I just stay here?";

                if (timer >= 0.0f) {
                    float t = 1.0f - (timer/1.1f);
                    int n = Max(1, t * txt.size());
                    rend = txt.substr(0, n);
                    timer -= get_deltaTime();
                } else {
                    rend = txt;
                }

                auto diag = std::make_shared<DialogueRender>(
                    rend, Vector2Int(pos.x - camX, p.y - 90),
                    game->window
                );
                game->dialogueRenders.push(diag);

                break;
            }

            case fox_town_2_1:
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::string txt = "1. You could've told me that before!";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(pos.x - camX, p.y - 30),
                            game->window
                        );
                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }
            
            case fox_town_2_2:
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::string txt = "2. Do you have any advice?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(pos.x - camX, p.y - 30),
                            game->window
                        );
                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case fox_town_2_3:
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::string txt = "3. Can I just stay here?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(pos.x - camX, p.y - 30),
                            game->window
                        );
                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }
        
            case bear_town_1:
            {
                std::string rend, txt = "1. Hello\n2. What are you doing?";

                if (timer >= 0.0f) {
                    float t = 1.0f - (timer/1.1f);
                    int n = Max(1, t * txt.size());
                    rend = txt.substr(0, n);
                    timer -= get_deltaTime();
                } else {
                    rend = txt;
                }

                auto diag = std::make_shared<DialogueRender>(
                    rend, Vector2Int(pos.x - camX, p.y - 60),
                    game->window
                );
                game->dialogueRenders.push(diag);

                break; 
            }

            case bear_town_1_1:
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::string txt = "1. Hello";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(pos.x - camX, p.y - 30),
                            game->window
                        );
                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case bear_town_1_2:
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::string txt = "2. What are you doing?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(pos.x - camX, p.y - 30),
                            game->window
                        );
                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case bear_town_2:
            {
                std::string rend, txt = "1. Yes\n2. No";

                if (timer >= 0.0f) {
                    float t = 1.0f - (timer/1.1f);
                    int n = Max(1, t * txt.size());
                    rend = txt.substr(0, n);
                    timer -= get_deltaTime();
                } else {
                    rend = txt;
                }

                auto diag = std::make_shared<DialogueRender>(
                    rend, Vector2Int(pos.x - camX, p.y - 60),
                    game->window
                );
                game->dialogueRenders.push(diag);

                break; 
            }

            case bear_town_2_1:
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::string txt = "1. Yes";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(pos.x - camX, p.y - 30),
                            game->window
                        );
                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }
            
            case bear_town_2_2:
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::string txt = "1. No";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(pos.x - camX, p.y - 30),
                            game->window
                        );
                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case bear_town_3:
            {
                std::string rend, txt = "1. Where do you get\nall these berries?\n2. How do you survive\nwith all these wolves?";

                if (timer >= 0.0f) {
                    float t = 1.0f - (timer/1.1f);
                    int n = Max(1, t * txt.size());
                    rend = txt.substr(0, n);
                    timer -= get_deltaTime();
                } else {
                    rend = txt;
                }

                auto diag = std::make_shared<DialogueRender>(
                    rend, Vector2Int(pos.x - camX, p.y - 120),
                    game->window
                );
                game->dialogueRenders.push(diag);

                break; 
            }

            case bear_town_3_1:
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::string txt = "1. Where do you get\nall these berries?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(pos.x - camX, p.y - 60),
                            game->window
                        );
                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case bear_town_3_2:
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::string txt = "2. How do you survive\nwith all these wolves?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(pos.x - camX, p.y - 60),
                            game->window
                        );
                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case bear_town_4:
            {
                std::string rend, txt = "1. Yeah!\n2. No thanks";

                if (timer >= 0.0f) {
                    float t = 1.0f - (timer/1.1f);
                    int n = Max(1, t * txt.size());
                    rend = txt.substr(0, n);
                    timer -= get_deltaTime();
                } else {
                    rend = txt;
                }

                auto diag = std::make_shared<DialogueRender>(
                    rend, Vector2Int(pos.x - camX, p.y - 60),
                    game->window
                );
                game->dialogueRenders.push(diag);

                break; 
            }

            case bear_town_4_1:
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::string txt = "1. Yeah!";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(pos.x - camX, p.y - 30),
                            game->window
                        );
                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }
            
            case bear_town_4_2:
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::string txt = "1. No Thanks";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(pos.x - camX, p.y - 30),
                            game->window
                        );
                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }
            
            case bear_town_5:
            {
                std::string rend, txt = "1. Sure thing!\n ";

                if (timer >= 0.0f) {
                    float t = 1.0f - (timer/1.1f);
                    int n = Max(1, t * txt.size());
                    rend = txt.substr(0, n);
                    timer -= get_deltaTime();
                } else {
                    rend = txt;
                }

                std::vector<std::string> vec = {rend, "2. What do you need ", " for?"};
                SDL_Color colour = {255,255,255,255};

                auto diag = std::make_shared<DialogueRender>(
                    vec, Vector2Int(pos.x-camX, p.y-60), game->window,
                    colour, Centred, &DialogueRender::bear_town_5_diag,
                    game->stoneTex
                );

                game->dialogueRenders.push(diag);
                break; 
            }

            case bear_town_5_1:
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::string txt = "1. Sure thing!";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(pos.x - camX, p.y - 30),
                            game->window
                        );
                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }
            
            case bear_town_5_2:
            {
                if (timer >= 0.0f) {
                    timer -= get_deltaTime();
                    if (timer < 1.0f) 
                    {
                        std::vector<std::string> vec = {"2. What do you need ", " for?"};
                        SDL_Color colour = {255,255,255,255};

                        auto diag = std::make_shared<DialogueRender>(
                            vec, Vector2Int(pos.x-camX, p.y-30), game->window,
                            colour, Centred, &DialogueRender::bear_town_5_2_diag,
                            game->stoneTex
                        );

                        game->dialogueRenders.push(diag);
                    }
                }
                break;
            }
        }
    }

}


void Game::handle_dialogue( int e )
{
    switch (currDialogue)
    {
        case test_dialogue:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(test_dialogue_2); 
                    currLevel->player->set_timer( 1.1f );
                    break;
                case SDLK_2:
                case SDLK_KP_2: 
                    enter_dialogue(test_dialogue_3); 
                    currLevel->player->set_timer( 1.1f );
                    break;
            } break;

        case fox_base_dialogue_1:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(fox_base_dialogue_1_1); 
                    currLevel->player->set_timer( 1.1f );
                    break;
                case SDLK_2:
                case SDLK_KP_2: 
                    enter_dialogue(fox_base_dialogue_1_2); 
                    currLevel->player->set_timer( 1.1f );
                    break;
            } break;

        case fox_base_dialogue_2:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(fox_base_dialogue_2_1); 
                    currLevel->player->set_timer( 1.1f );
                    break;
            } break;

        case fox_base_dialogue_3:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(fox_base_dialogue_3_1); 
                    currLevel->player->set_timer( 1.1f );
                    break;
                case SDLK_2:
                case SDLK_KP_2: 
                    enter_dialogue(fox_base_dialogue_3_2); 
                    currLevel->player->set_timer( 1.1f );
                    break;
            } break;
    
        case fox_town_1:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(fox_town_1_1); 
                    currLevel->player->set_timer( 1.1f );
                    break;
                case SDLK_2:
                case SDLK_KP_2: 
                    enter_dialogue(fox_town_1_2); 
                    currLevel->player->set_timer( 1.1f );
                    break;
                case SDLK_3:
                case SDLK_KP_3:
                    enter_dialogue(fox_town_1_3);
                    currLevel->player->set_timer( 1.1f );
                    break;
            } break;

        case fox_town_2:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(fox_town_2_1); 
                    currLevel->player->set_timer( 1.1f );
                    break;
                case SDLK_2:
                case SDLK_KP_2: 
                    enter_dialogue(fox_town_2_2); 
                    currLevel->player->set_timer( 1.1f );
                    break;
                case SDLK_3:
                case SDLK_KP_3:
                    enter_dialogue(fox_town_2_3);
                    currLevel->player->set_timer( 1.1f );
                    break;
            } break;

        case bear_town_1:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(bear_town_1_1); 
                    currLevel->player->set_timer( 1.1f );
                    break;
                case SDLK_2:
                case SDLK_KP_2: 
                    enter_dialogue(bear_town_1_2); 
                    currLevel->player->set_timer( 1.1f );
                    break;
            } break;
        
        case bear_town_2:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(bear_town_2_1); 
                    currLevel->player->set_timer( 1.1f );
                    break;
                case SDLK_2:
                case SDLK_KP_2: 
                    enter_dialogue(bear_town_2_2); 
                    currLevel->player->set_timer( 1.1f );
                    break;
            } break;
        
        case bear_town_3:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(bear_town_3_1); 
                    currLevel->player->set_timer( 1.1f );
                    break;
                case SDLK_2:
                case SDLK_KP_2: 
                    enter_dialogue(bear_town_3_2); 
                    currLevel->player->set_timer( 1.1f );
                    break;
            } break;
        
        case bear_town_4:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(bear_town_4_1); 
                    currLevel->player->set_timer( 1.1f );
                    break;
                case SDLK_2:
                case SDLK_KP_2: 
                    enter_dialogue(bear_town_4_2); 
                    currLevel->player->set_timer( 1.1f );
                    break;
            } break;
        
        case bear_town_5:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(bear_town_5_1); 
                    currLevel->player->set_timer( 1.1f );
                    break;
                case SDLK_2:
                case SDLK_KP_2: 
                    enter_dialogue(bear_town_5_2); 
                    currLevel->player->set_timer( 1.1f );
                    break;
            } break;
    }
}











DialogueRender::DialogueRender(std::string txt, Vector2Int pos, std::shared_ptr<LWindow> window, SDL_Color colour, TextOrientation orientation, void (DialogueRender::*func)(), std::shared_ptr<LTexture> tex0, std::shared_ptr<LTexture> tex1)
: pos(pos), window(window), colour(colour), orientation(orientation), renderFunc(func), texture0(tex0), texture1(tex1)
{
    strings.push_back(txt);
}

DialogueRender::DialogueRender(std::vector<std::string> strings, Vector2Int pos, std::shared_ptr<LWindow> window, SDL_Color colour, TextOrientation orientation, void (DialogueRender::*func)(), std::shared_ptr<LTexture> tex0, std::shared_ptr<LTexture> tex1)
: strings(strings), pos(pos), window(window), colour(colour), orientation(orientation), renderFunc(func), texture0(tex0), texture1(tex1) {}


void DialogueRender::render()
{
    (this->*renderFunc)();
}

void DialogueRender::defaultTextRender()
{
    renderText(strings[0], pos.x, pos.y, window);
}

void DialogueRender::bear_town_5_diag()
{
    if (strings.size() != 3) return;

    renderText(strings[0], pos.x, pos.y, window);

    std::string txt0 = strings[1], txt1 = strings[2];
    auto tex0 = std::make_unique<LTexture>(window),
         tex1 = std::make_unique<LTexture>(window); 
    
    tex0->loadFromRenderedText(txt0, colour);
    tex1->loadFromRenderedText(txt1, colour);

    int w = tex0->getWidth()+tex1->getWidth()+30,
        x = pos.x - (w/2), y = pos.y+30;
    SDL_Rect rect = {0, 0, 30, 30};

    tex0->render(x, y); x += tex0->getWidth();
    texture0->render(x, y, &rect); x += 30;
    tex1->render(x, y); 
}

void DialogueRender::bear_town_5_2_diag()
{
    if (strings.size() != 2) return;

    std::string txt0 = strings[0], txt1 = strings[1];
    auto tex0 = std::make_unique<LTexture>(window),
         tex1 = std::make_unique<LTexture>(window); 
    
    tex0->loadFromRenderedText(txt0, colour);
    tex1->loadFromRenderedText(txt1, colour);

    int w = tex0->getWidth()+tex1->getWidth()+30,
        x = pos.x - (w/2), y = pos.y;
    SDL_Rect rect = {0, 0, 30, 30};

    tex0->render(x, y); x += tex0->getWidth();
    texture0->render(x, y, &rect); x += 30;
    tex1->render(x, y); 
}

void DialogueRender::bear_town_case14()
{
    if (strings.size() != 2) return;

    std::string txt0 = strings[0], txt1 = strings[1]; 

    auto tex0 = std::make_unique<LTexture>(window),
         tex1 = std::make_unique<LTexture>(window); 
    
    tex0->loadFromRenderedText(txt0, colour);
    tex1->loadFromRenderedText(txt1, colour);

    int w = tex0->getWidth()+tex1->getWidth() + 60,
        x = pos.x - (w/2), y = pos.y;
    SDL_Rect rect = {0, 0, 30, 30};

    tex0->render(x, y); x += tex0->getWidth();
    texture0->render(x, y, &rect); x += 30;
    tex1->render(x, y); x += tex1->getWidth();
    texture1->render(x, y, &rect);
}

void DialogueRender::fox_base_case11()
{
    if (strings.size() != 4) return;

    renderText(strings[0], pos.x, pos.y, window);

    std::string txt0 = strings[1], txt1 = strings[2], txt2 = strings[3];
    auto tex0 = std::make_unique<LTexture>(window),
         tex1 = std::make_unique<LTexture>(window),
         tex2 = std::make_unique<LTexture>(window);

    tex0->loadFromRenderedText(txt0, colour);
    tex1->loadFromRenderedText(txt1, colour);
    tex2->loadFromRenderedText(txt2, colour);

    int w = tex0->getWidth()+tex1->getWidth()+tex2->getWidth()+60,
        x = pos.x - (w/2), y = pos.y + 30;
    SDL_Rect rect = {0, 0, 30, 30};

    tex0->render(x, y); x += tex0->getWidth();
    texture0->render(x, y, &rect); x += 30;
    tex1->render(x, y); x += tex1->getWidth();
    texture1->render(x, y, &rect); x += 30;
    tex2->render(x, y);
}