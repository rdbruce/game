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

    switch (hp)
    {
        case 2: { // first dialogue option
            // if the next dialogue stage has been reached
            if (game->currDialogue == test_dialogue_2 || 
                game->currDialogue == test_dialogue_3) 
            {
                hp = 4; timer = 0.25f; break;
            }

            std::string rend;
            std::string txt = "Hey! I'm talking to you!";
            if (timer >= 0.0f) {
                float t = 1.0f - (timer/0.25f);
                int n = Max(1, t * txt.size());
                rend = txt.substr(0, n);
                timer -= get_deltaTime();
            } else {
                rend = txt;
                game->enter_dialogue(test_dialogue);
            }

            // exit dialogue if the player walks too far away
            Vector2 pPos = get_player_pos();
            float dist = (pPos - pos).length();
            if (dist > 1.5f*game->interactRange) {
                game->enter_dialogue(None);
                hp = 1; break;
            }

            // load text to be rendered
            if (!altTex->loadFromRenderedText(rend, {255,255,255,255})) {
                std::cerr << "Unable to render text!" << std::endl;
            }
            int w = altTex->getWidth(), h = altTex->getHeight(), dx = (hitbox.w-w)/2;
            altTex->render( p.x + dx, p.y-h );
            break;
        }

        case 3: // go back
            hp = 2; break;

        case 4: {
            std::string rend;
            std::string txt = (game->currDialogue==test_dialogue_2)? 
                "great to hear!" : "aw man what the heck dude :(";
            if (timer >= 0.0f) {
                float t = 1.0f - (timer/0.25f);
                int n = Max(1, t * txt.size());
                rend = txt.substr(0, n);
                timer -= get_deltaTime();
            } else {
                rend = txt;
            }

            // exit dialogue if the player walks too far away
            Vector2 pPos = get_player_pos();
            float dist = (pPos - pos).length();
            if (dist > 1.5f*game->interactRange) {
                game->enter_dialogue(None);
                hp = 1; break;
            }

            // load text to be rendered
            if (!altTex->loadFromRenderedText(rend, {255,255,255,255})) {
                std::cerr << "Unable to render text!" << std::endl;
            }
            int w = altTex->getWidth(), h = altTex->getHeight(), dx = (hitbox.w-w)/2;
            altTex->render( p.x + dx, p.y-h );
            break;
        }

        case 5:
            game->enter_dialogue(None);
            hp = 1; break;
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
        case test_dialogue: {
            float t = 1.0f - (timer/0.25f);

            std::string rend;
            std::string txt = "1. I'm responding to you!";
            if (timer >= 0.0f) {
                int n = Max(1, t * txt.size());
                rend = txt.substr(0, n);
                timer -= get_deltaTime();
            } else {
                rend = txt;
            }


            if (!altTex->loadFromRenderedText(rend, {255,255,255,255})) {
                std::cerr << "Unable to render text!" << std::endl;
            }
            int w = altTex->getWidth(), h = altTex->getHeight(), dx = (hitbox.w-w)/2;
            altTex->render( p.x + dx, p.y-2*h );

            txt = "2. I'm responding to you! (rudely)";
            if (timer >= 0.0f) {
                int n = Max(1, t * txt.size());
                rend = txt.substr(0, n);
                timer -= get_deltaTime();
            } else {
                rend = txt;
            }

            if (!altTex->loadFromRenderedText(rend, {255,255,255,255})) {
                std::cerr << "Unable to render text!" << std::endl;
            }
            w = altTex->getWidth(); h = altTex->getHeight(); dx = (hitbox.w-w)/2;
            altTex->render( p.x + dx, p.y-h );
            break;
        }

        case test_dialogue_2: {
            if (timer >= 0.0f) { 
                timer -= get_deltaTime();
                if (timer < 1.0f) {
                    std::string txt = "1. I'm responding to you!";
                    if (!altTex->loadFromRenderedText(txt, {255,255,255,255})) {
                        std::cerr << "Unable to render text!" << std::endl;
                    }
                    int w = altTex->getWidth(), h = altTex->getHeight(), dx = (hitbox.w-w)/2;
                    altTex->render( p.x + dx, p.y-h );
                }
            }
            break;
        }

        case test_dialogue_3: {
            if (timer >= 0.0f) {
                timer -= get_deltaTime();
                if (timer < 1.0f) {
                    std::string txt = "2. I'm responding to you! (rudely)";
                    if (!altTex->loadFromRenderedText(txt, {255,255,255,255})) {
                        std::cerr << "Unable to render text!" << std::endl;
                    }
                    int w = altTex->getWidth(), h = altTex->getHeight(), dx = (hitbox.w-w)/2;
                    altTex->render( p.x + dx, p.y-h );
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
                    enter_dialogue(test_dialogue_2); 
                    currLevel->player->set_timer( 1.1f );
                    break;
                case SDLK_KP_1: 
                    enter_dialogue(test_dialogue_2); 
                    currLevel->player->set_timer( 1.1f );
                    break;
                case SDLK_2:    
                    enter_dialogue(test_dialogue_3); 
                    currLevel->player->set_timer( 1.1f );
                    break;
                case SDLK_KP_2: 
                    enter_dialogue(test_dialogue_3); 
                    currLevel->player->set_timer( 1.1f );
                    break;
            } break;
    }
}