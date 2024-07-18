#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <sstream>
#include <stdio.h>
#include <string>

#include "engine/LTexture.hpp"
#include "engine/LTimer.hpp"
#include "engine/LWindow.hpp"
#include "engine/GameMath.hpp"

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

// Generic thread wrapper
class Simulator
{
public:
  using Callback = std::function<void()>;

private:
  Callback callback;
  bool die;
  std::thread tid;
  std::string name;

public:
  // Construct std function from provided callable
  Simulator(Callback _callback) : callback(_callback), die(false)
  {
    // Create thread
    tid = std::thread([this]()
                      { this->worker(); });
  }

  ~Simulator()
  {
    {
      die = true;
    }
    tid.join();
  }

  // Generic run loop that pMath exists in
  void worker()
  {
    using namespace std;
    for (;;)
    {
      callback();
      if (die)
        return;
    }
  }
};

// Generic SafeSharedPtr
template <typename T>
class SafeSharedPtr
{
  std::mutex mtx;
  std::shared_ptr<T> ptr;

public:
  SafeSharedPtr() : ptr(nullptr) {}

  void store(std::shared_ptr<T> arg)
  {
    std::unique_lock<std::mutex> lock(mtx);
    ptr = arg;
  }

  std::shared_ptr<T> load()
  {
    std::unique_lock<std::mutex> lock(mtx);
    return ptr;
  }
};

int main(
    // int argc, char* args[]
)
{
  // Initialize SDL
  auto gHolder = std::make_shared<LWindow>();

  GameMenu menu(gHolder);

  Game game(gHolder);

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
      menu.handle_events(e, &inMenu);
      if (!inMenu) game.handle_events(e);
    }

    game.update_deltaTime();
    if (!inMenu) {

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

      if (!inMenu) {
        // Render background
        game.center_camera_on_player();
        game.render_background();
        game.render_gameobjects();
        game.render_overlay();
        game.render_cell_health();
        game.render_player_health();

        game.render_framerate();

      } else {
        menu.render_background();
        menu.render_buttons();
      }

      // Update screen
      SDL_RenderPresent(gHolder->gRenderer);
    }
  }
  return 0;
}