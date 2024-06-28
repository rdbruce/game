#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <sstream>
#include <stdio.h>
#include <string>

#include "Dot.hpp"
#include "LTexture.hpp"
#include "LTimer.hpp"
#include "SDLHolder.hpp"

#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

// Generic thread wrapper
class Simulator {
public:
  using Callback = std::function<void()>;

private:
  Callback callback;
  bool die;
  std::thread tid;
  std::string name;

public:
  // Construct std function from provided callable
  Simulator(Callback _callback) : callback(_callback), die(false) {
    // Create thread
    tid = std::thread([this]() { this->worker(); });
  }

  ~Simulator() {
    { die = true; }
    tid.join();
  }

  // Generic run loop that pMath exists in
  void worker() {
    using namespace std;
    for (;;) {
      callback();
      if (die)
        return;
    }
  }
};

// Generic SafeSharedPtr
template <typename T> class SafeSharedPtr {
  std::mutex mtx;
  std::shared_ptr<T> ptr;

public:
  SafeSharedPtr() : ptr(nullptr) {}

  void store(std::shared_ptr<T> arg) {
    std::unique_lock<std::mutex> lock(mtx);
    ptr = arg;
  }

  std::shared_ptr<T> load() {
    std::unique_lock<std::mutex> lock(mtx);
    return ptr;
  }
};

int main(
    // int argc, char* args[]
) {

  // The dimensions of the level
  const int LEVEL_WIDTH = 1280;
  const int LEVEL_HEIGHT = 960;

  // Screen dimension constants
  const int SCREEN_FPS = 60;
  const int SCREEN_TICK_PER_FRAME = 1000 / SCREEN_FPS;
  const int SCREEN_WIDTH = 640;
  const int SCREEN_HEIGHT = 480;

  // Initialize SDL
  auto gHolder = std::make_shared<SDLHolder>(SCREEN_WIDTH, SCREEN_HEIGHT);

  // Scene textures
  auto gFPSTextTexture = std::make_shared<LTexture>(gHolder);
  auto gDotTexture = std::make_shared<LTexture>(gHolder);
  auto gBGTexture = std::make_shared<LTexture>(gHolder);

  // Load dot texture
  if (!gDotTexture->loadFromFile("../../assets/dot.bmp")) {
    printf("Failed to load dot texture!\n");
  }

  // Load background texture
  if (!gBGTexture->loadFromFile("../../assets/bg.png")) {
    printf("Failed to load background texture!\n");
  }

  // The dot that will be moving around on the screen
  Dot dot(gDotTexture);

  // The camera area
  SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

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

  // Main loop flag
  bool quit = false;

  // Start counting frames per second
  int countedFrames = 0;
  fpsTimer.start();

  // While application is running
  while (!quit) {
    // Start cap timer
    capTimer.start();

    // Handle events on queue
    while (SDL_PollEvent(&e) != 0) {
      // User requests quit
      if (e.type == SDL_QUIT) {
        quit = true;
      }

      // Handle input for the dot
      dot.handleEvent(e);
    }

    // Move the dot
    dot.move();

    // Center the camera over the dot
    camera.x = (dot.getPosX() + Dot::DOT_WIDTH / 2) - SCREEN_WIDTH / 2;
    camera.y = (dot.getPosY() + Dot::DOT_HEIGHT / 2) - SCREEN_HEIGHT / 2;

    // Keep the camera in bounds
    if (camera.x < 0) {
      camera.x = 0;
    }
    if (camera.y < 0) {
      camera.y = 0;
    }
    if (camera.x > LEVEL_WIDTH - camera.w) {
      camera.x = LEVEL_WIDTH - camera.w;
    }
    if (camera.y > LEVEL_HEIGHT - camera.h) {
      camera.y = LEVEL_HEIGHT - camera.h;
    }

    // Calculate and correct fps
    float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
    if (avgFPS > 2000000) {
      avgFPS = 0;
    }

    // Render text
    if (!gFPSTextTexture->loadFromRenderedText(timeText.str().c_str(),
                                               textColor)) {
      printf("Unable to render FPS texture!\n");
    }

    // Clear screen
    SDL_SetRenderDrawColor(gHolder->gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gHolder->gRenderer);

    // Render background
    gBGTexture->render(0, 0, &camera);

    // Render dot objects
    dot.render(camera.x, camera.y);

    // Set text to be rendered
    timeText.str("");
    timeText << "Average Frames Per Second (With Cap) " << avgFPS;
    // Render text
    gFPSTextTexture->render(0, 0);

    // Update screen
    SDL_RenderPresent(gHolder->gRenderer);
    ++countedFrames;

    // If frame finished early
    int frameTicks = capTimer.getTicks();
    if (frameTicks < SCREEN_TICK_PER_FRAME) {
      // Wait remaining time
      SDL_Delay(SCREEN_TICK_PER_FRAME - frameTicks);
    }
  }
  return 0;
}
