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
  auto gHolder = std::make_shared<LWindow>();

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
      game.update_gameobjects();

      // call update_cells first
      game.update_cells();
      game.dayNightCycle();
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
        // std::cout << "centring camera on player\n";
        game.center_camera_on_player();
        // std::cout << "rendering background\n";
        game.render_background();
        // std::cout << "rendering gameobjects\n";
        game.render_gameobjects();
        // std::cout << "rendering overlay\n";
        game.render_overlay();
        // std::cout << "rendering gameobjects under trees\n";
        game.render_gameobjects_under_trees();
        // std::cout << "rendering darkness\n";
        game.render_darkness();

        // UI rendering
        // std::cout << "rendering cell health\n";
        game.render_cell_health();
        // std::cout << "rendering dialogue\n";
        game.render_dialogue();
        // std::cout << "rendering player health\n";
        game.render_player_health();
        // std::cout << "rendering clock\n";
        game.render_clock();
        // std::cout << "rendering controls\n";
        if (!inMenu) game.render_controls();
        // std::cout << "rendering framerate\n";
        game.render_framerate();
      }

      if (inMenu) {
        menu.render_background();
        menu.render_highscores();
        menu.render_buttons();
        menu.render_confirmation();
      }
      menu.render_CRT();
      menu.render_aspect_ratio();

      // Update screen
      SDL_RenderPresent(gHolder->gRenderer);
    }
  }
  return 0;
}