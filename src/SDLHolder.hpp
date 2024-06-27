#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <sstream>
#include <stdio.h>
#include <string>

// SDL Init Object
class SDLHolder {
public:
  // The window we'll be rendering to
  SDL_Window *gWindow = NULL;

  // The window renderer
  SDL_Renderer *gRenderer = NULL;

  // Globally used font
  TTF_Font *gFont = NULL;

  // Initializes variables
  SDLHolder(const int wFromMain, const int hFromMain);

  // Deallocates memory
  ~SDLHolder();
};
