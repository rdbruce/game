#pragma once

#include "LWindow.hpp"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <memory>

// Texture wrapper class
class LTexture
{
public:
  // Initializes variables
  LTexture(std::shared_ptr<LWindow> gHolder);

  // Deallocates memory
  ~LTexture();

  // Loads image at specified path
  bool loadFromFile(std::string path);

#if defined(SDL_TTF_MAJOR_VERSION)
  // Creates image from font string
  bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
#endif

  // Deallocates texture
  void free();

  // Set color modulation
  void setColor(Uint8 red, Uint8 green, Uint8 blue);

  // Set blending
  void setBlendMode(SDL_BlendMode blending);

  // Set alpha modulation
  void setAlpha(Uint8 alpha);

  // Renders to fit the whole background
  void LTexture::renderAsBackground(SDL_Rect *clip=NULL, double angle =0.0,
                      SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

  // Renders texture at given point
  void render(int x, int y, SDL_Rect *clip = NULL, double angle = 0.0,
              SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

  // Gets image dimensions
  int getWidth();
  int getHeight();

private:
  // The actual hardware texture
  SDL_Texture *mTexture;

  std::shared_ptr<LWindow> gHolder;

  // Image dimensions
  int mWidth;
  int mHeight;
};