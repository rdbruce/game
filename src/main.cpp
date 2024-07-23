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

  auto audio = std::make_shared<LAudio>();
  if (!audio->loadFromFile("../../assets/Audio/ThinkFastChucklenuts.mp3")) {
    std::cerr << "failed to load audio" << std::endl;
  }

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

    audio->play();

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
      SDL_SetRenderDrawColor(gHolder->gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
      SDL_RenderClear(gHolder->gRenderer);

      // Render background
      if (menu.is_inGame()) 
      {
        game.center_camera_on_player();
        game.render_background();
        game.render_gameobjects();
        game.render_overlay();
        game.render_cell_health();
        game.render_player_health();
        if (!inMenu) game.render_controls();

        game.render_framerate();
      }

      if (inMenu) {
        menu.render_background();
        menu.render_buttons();
        menu.render_confirmation();
      }

      // Update screen
      SDL_RenderPresent(gHolder->gRenderer);
    }
  }
  return 0;
}