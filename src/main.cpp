// prevents the console from opening on application startup
// #pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <SDL_mixer.h>

#include "engine/LTexture.hpp"
#include "engine/LTimer.hpp"
#include "engine/LWindow.hpp"
#include "engine/GameMath.hpp"
#include "engine/LAudio.hpp"

#include "game/Game.hpp"

#include "gameObject/GameObject.hpp"

#include "Menu/Menu.hpp"
#include "Menu/Button.hpp"

#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>


int main(
    // int argc, char* args[]
)
{
  // Initialize SDL
  auto gHolder = std::make_shared<LWindow>(1024, 1024, "A Dam Good Game");

  Game game(gHolder);
  GameMenu menu(gHolder, &game);

  // Event handler
  SDL_Event e;

  // Main loop flag
  bool quit = false;
  bool inMenu = true;

  // While application is running
  while (!quit)
  {
    // Handle events on queue
    while (SDL_PollEvent(&e) != 0)
    {
      // User requests quit
      if (e.type == SDL_QUIT)
      {
        quit = true;
      }

      // Handle window events
      gHolder->handleEvent(e);
      quit = menu.handle_events(e, &inMenu);
      if (!inMenu) game.handle_events(e);
    }

    menu.update();
    game.update_deltaTime();
    if (!inMenu) 
    {
      // std::cout << "updating game objects\n";
      game.update_gameobjects();
      // call update_cells first
      // std::cout << "updating cells\n";
      game.update_cells();
      // std::cout << "updating day/night\n";
      game.dayNightCycle();
      // std::cout << "attempting enemy spawn\n";
      game.attempt_enemy_spawn();
    }

    // Only draw when not minimized
    if (!gHolder->isMinimized())
    {
      // Clear screen
      SDL_SetRenderDrawColor(gHolder->gRenderer, 0, 0, 0, 0xFF);
      SDL_RenderClear(gHolder->gRenderer);

      // Render background
      if (menu.is_inGame()) 
      {
        // std::cout << "centring camera\n";
        game.center_camera_on_player();
        // std::cout << "rendering background\n";
        game.render_background();
        // std::cout << "rendering gameobjects\n";
        game.render_gameobjects();
        // std::cout << "rendering overlay\n";
        game.render_overlay();
        // std::cout << "rendering objects under trees\n";
        game.render_gameobjects_under_trees();
        // std::cout << "rendering darkness\n";
        game.render_darkness();

        // UI rendering
        // std::cout << "rendering cell health\n";
        game.render_cell_health();
        game.render_dialogue();
        game.render_player_health();
        game.render_clock();
        if (!inMenu) game.render_controls();
      }

      if (inMenu) {
        menu.render_background();
        menu.render_highscores();
        menu.render_settings();
        menu.render_buttons();
        menu.render_confirmation();
        menu.render_credits();
      }
      menu.render_FPS();
      menu.render_CRT();
      menu.render_aspect_ratio();

      // Update screen
      SDL_RenderPresent(gHolder->gRenderer);
    }
  }
  return 0;
}