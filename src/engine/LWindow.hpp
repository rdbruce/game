#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <sstream>
#include <stdio.h>
#include <string>

// SDL Init Object
class LWindow
{
public:
  // The window renderer
  SDL_Renderer *gRenderer = NULL;

  // Globally used font
  TTF_Font *gFont = NULL;

  // Handles window events
  void handleEvent(SDL_Event &e);

  // Window dimensions
  int getWidth();
  int getHeight();

  float getScaleX();
  float getScaleY();

  // Window focii
  bool hasMouseFocus();
  bool hasKeyboardFocus();
  bool isMinimized();

  void toggleFullscreen();

  // Initializes variables
  LWindow();

  // Deallocates memory
  ~LWindow();

private:
  // The window we'll be rendering to
  SDL_Window *gWindow = NULL;

  // Window dimensions
  int wWidth;
  int wHeight;

  // scaling factor
  float scaleX = 1.0f;
  float scaleY = 1.0f;

  // Window focus
  bool wMouseFocus;
  bool wKeyboardFocus;
  bool wFullScreen;
  bool wMinimized;
};