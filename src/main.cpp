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
  // Screen dimension constants
  const int SCREEN_FPS = 60;
  const int SCREEN_TICK_PER_FRAME = 1000 / SCREEN_FPS;

  double angle = 0;

  // Initialize SDL
  auto gHolder = std::make_shared<LWindow>();

  // Scene textures
  auto gFPSTextTexture = std::make_shared<LTexture>(gHolder);
  auto gMouseTextTexture = std::make_shared<LTexture>(gHolder);
  auto gBGTexture = std::make_shared<LTexture>(gHolder);

  // Load background texture
  if (!gBGTexture->loadFromFile("../../assets/Bert.png"))
  {
    printf("Failed to load background texture!\n");
  }

  Game game(gHolder);

  // Event handler
  SDL_Event e;

  // Set text color as black
  SDL_Color textColor = {0, 0, 0, 255};

  // The frames per second timer
  LTimer fpsTimer;

  // The frames per second cap timer
  LTimer capTimer;

  // In memory text stream
  std::stringstream timeText;
  std::stringstream mouseText;

  // Main loop flag
  bool quit = false;

  // Start counting frames per second
  int countedFrames = 0;
  fpsTimer.start();

  // While application is running
  while (!quit)
  {
    // Start cap timer
    capTimer.start();

    angle += 0.1;

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
      game.handle_events(e);
    }


    game.update_deltaTime();

    game.update_gameobjects();

    // Calculate and correct fps
    float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
    if (avgFPS > 2000000)
    {
      avgFPS = 0;
    }

    // Set text to be rendered
    timeText.str("");
    timeText << "Average Frames Per Second (With Cap) " << avgFPS;
    // Render text
    if (!gFPSTextTexture->loadFromRenderedText(timeText.str().c_str(),
                                               textColor))
    {
      printf("Unable to render FPS texture!\n");
    }

    // Set text to be rendered
    mouseText.str("");
    int x, y;
    SDL_GetMouseState(&x, &y);
    mouseText << "Mouse is at (" << x << "," << y << ")";
    // Render text
    if (!gMouseTextTexture->loadFromRenderedText(mouseText.str().c_str(),
                                                 textColor))
    {
      printf("Unable to render FPS texture!\n");
    }

    // Only draw when not minimized
    if (!gHolder->isMinimized())
    {
      // Clear screen
      SDL_SetRenderDrawColor(gHolder->gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
      SDL_RenderClear(gHolder->gRenderer);


      // Render background
      game.render_background();
      game.render_gameobjects();


      gFPSTextTexture->render(0, 0);
      gMouseTextTexture->render(0, 30);

      // Update screen
      ++countedFrames;
      SDL_RenderPresent(gHolder->gRenderer);
    }

    // If frame finished early
    int frameTicks = capTimer.getTicks();
    if (frameTicks < SCREEN_TICK_PER_FRAME)
    {
      // Wait remaining time
      SDL_Delay(SCREEN_TICK_PER_FRAME - frameTicks);
    }
  }
  return 0;
}