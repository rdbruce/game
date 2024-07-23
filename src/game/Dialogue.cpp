#include "../gameObject/GameObject.hpp"
#include "Game.hpp"
#include "Dialogue.hpp"

void GameObject::foxRenderFunc( int camX, int camY )
{
    Vector2Int p( hitbox.x-camX, hitbox.y-camY );
    // not within the camera's view, don't render
    if (p.x != Clamp(-hitbox.w, game->camera.w, p.x) || p.y != Clamp(-hitbox.h, game->camera.h, p.y)) {
        return;
    }
    tex->render( p.x, p.y, &hitbox );

    // exit dialogue if the player walks too far away
    Vector2 pPos = get_player_pos();
    float dist = (pPos - pos).length();
    if (dist > 1.5f*game->interactRange) {
        game->enter_dialogue(None);
        hp = 1; return;
    }


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

                renderText(rend, pos.x - camX, p.y - 30, game->window);
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

                renderText(rend, pos.x - camX, p.y - 90, game->window);
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

                renderText(rend, pos.x - camX, p.y - 30, game->window);
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

                renderText(rend, pos.x - camX, p.y - 90, game->window);
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

                renderText(rend, pos.x - camX, p.y - 30, game->window);
                break;
            }

            case 11:
            {
                std::string rend, txt = "Look just put [recipe] together\nand figure it out";
                if (timer >= 0.0f) {
                    float t = 1.0f - (timer/0.25f);
                    int n = Max(1, t * txt.size());
                    rend = txt.substr(0, n);
                    timer -= get_deltaTime();
                } else {
                    rend = txt;
                }

                renderText(rend, pos.x - camX, p.y - 60, game->window);
                break;
            }

            case 12:
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

                renderText(rend, pos.x - camX, p.y - 30, game->window);
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

                renderText(rend, pos.x - camX, p.y - 30, game->window);
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

                renderText(rend, pos.x - camX, p.y - 30, game->window);
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

                renderText(rend, pos.x - camX, p.y - 90, game->window);
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

                renderText(rend, pos.x - camX, p.y - 30, game->window);
                break;
            }

            case 8:
                hp = 5; timer = 0.25f;
                break;

            case 9:
            {
                std::string rend, txt = "Oh those pricks...";
                if (timer >= 0.0f) {
                    float t = 1.0f - (timer/0.25f);
                    int n = Max(1, t * txt.size());
                    rend = txt.substr(0, n);
                    timer -= get_deltaTime();
                } else {
                    rend = txt;
                }

                renderText(rend, pos.x - camX, p.y - 30, game->window);
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

                renderText(rend, pos.x - camX, p.y - 60, game->window);
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

                renderText(rend, pos.x - camX, p.y - 30, game->window);
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

                renderText(rend, pos.x - camX, p.y - 30, game->window);
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

                renderText(rend, pos.x - camX, p.y - 30, game->window);
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

                renderText(rend, pos.x - camX, p.y - 60, game->window);
                break;
            }

            case 18:
                hp = 1;
                game->enter_dialogue(None);
                break;
        }
    }
}


void GameObject::playerRenderFunc( int camX, int camY )
{
    Vector2Int p( hitbox.x-camX, hitbox.y-camY );
    // not within the camera's view, don't render
    if (p.x != Clamp(-hitbox.w, game->camera.w, p.x) || p.y != Clamp(-hitbox.h, game->camera.h, p.y)) {
        return;
    }
    tex->render( p.x, p.y, &hitbox );

    switch (game->currDialogue)
    {
        case fox_base_dialogue_1: 
        {
            std::string rend, txt = "1. Where is here?\n2. What are YOU doing here?";

            if (timer >= 0.0f) {
                float t = 1.0f - (timer/0.25f);
                int n = Max(1, t * txt.size());
                rend = txt.substr(0, n);
                timer -= get_deltaTime();
            } else {
                rend = txt;
            }

            renderText(txt, pos.x-camX, p.y-60, game->window);
            break;
        }

        case fox_base_dialogue_1_1: 
        {
            if (timer >= 0.0f) {
                timer -= get_deltaTime();
                if (timer < 1.0f) 
                {
                    std::string txt = "1. Where is here?";
                    renderText(txt, pos.x-camX, p.y-30, game->window);
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
                    renderText(txt, pos.x-camX, p.y-30, game->window);
                }
            }
            break;
        }

        case fox_base_dialogue_2:
        {
            std::string rend, txt = "1. But how do I\nget past that river?";

            if (timer >= 0.0f) {
                float t = 1.0f - (timer/0.25f);
                int n = Max(1, t * txt.size());
                rend = txt.substr(0, n);
                timer -= get_deltaTime();
            } else {
                rend = txt;
            }

            renderText(txt, pos.x-camX, p.y-60, game->window);
            break;
        }

        case fox_base_dialogue_2_1:
        {
            if (timer >= 0.0f) {
                timer -= get_deltaTime();
                if (timer < 1.0f) 
                {
                    std::string txt = "1. But how do I\nget past that river?";
                    renderText(txt, pos.x-camX, p.y-60, game->window);
                }
            }
            break;
        }

        case fox_base_dialogue_3:
        {
            std::string rend, txt = "1. How do I do THAT?\n2. How are YOU getting there?";

            if (timer >= 0.0f) {
                float t = 1.0f - (timer/0.25f);
                int n = Max(1, t * txt.size());
                rend = txt.substr(0, n);
                timer -= get_deltaTime();
            } else {
                rend = txt;
            }

            renderText(txt, pos.x-camX, p.y-60, game->window);
            break;
        }

        case fox_base_dialogue_3_1:
        {
            if (timer >= 0.0f) {
                timer -= get_deltaTime();
                if (timer < 1.0f) 
                {
                    std::string txt = "1. How do I do THAT?";
                    renderText(txt, pos.x-camX, p.y-30, game->window);
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
                    renderText(txt, pos.x-camX, p.y-30, game->window);
                }
            }
            break;
        }

        case fox_town_1:
        {
            std::string rend, txt = "1. What is this place?\n2. How did YOU make it up here?\n3. Who were all those wolves?";

            if (timer >= 0.0f) {
                float t = 1.0f - (timer/0.25f);
                int n = Max(1, t * txt.size());
                rend = txt.substr(0, n);
                timer -= get_deltaTime();
            } else {
                rend = txt;
            }

            renderText(txt, pos.x-camX, p.y-90, game->window);
            break;
        }

        case fox_town_1_1:
        {
            if (timer >= 0.0f) {
                timer -= get_deltaTime();
                if (timer < 1.0f) 
                {
                    std::string txt = "1. What is this place?";
                    renderText(txt, pos.x-camX, p.y-30, game->window);
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
                    renderText(txt, pos.x-camX, p.y-30, game->window);
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
                    renderText(txt, pos.x-camX, p.y-30, game->window);
                }
            }
            break;
        }

        case fox_town_2:
        {
            std::string rend, txt = "1. You could've told me that before!\n2. Do you have any advice?\n3. Can I just stay here?";

            if (timer >= 0.0f) {
                float t = 1.0f - (timer/0.25f);
                int n = Max(1, t * txt.size());
                rend = txt.substr(0, n);
                timer -= get_deltaTime();
            } else {
                rend = txt;
            }

            renderText(txt, pos.x-camX, p.y-90, game->window);
            break;
        }

        case fox_town_2_1:
        {
            if (timer >= 0.0f) {
                timer -= get_deltaTime();
                if (timer < 1.0f) 
                {
                    std::string txt = "1. You could've told me that before!";
                    renderText(txt, pos.x-camX, p.y-30, game->window);
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
                    renderText(txt, pos.x-camX, p.y-30, game->window);
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
                    renderText(txt, pos.x-camX, p.y-30, game->window);
                }
            }
            break;
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
    }
}