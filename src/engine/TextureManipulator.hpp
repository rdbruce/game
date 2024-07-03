#pragma once

#include <SDL.h>
#include <iostream>

#include "LTexture.hpp"



// a class that is able to manipulate textures
class TextureManipulator
{

    public:

        TextureManipulator();

        // renders one LTexture onto the desired region of another
        void renderTextureToTexture( std::shared_ptr<LTexture> target, std::shared_ptr<LTexture> source, SDL_Rect *targetRect );
        void renderTextureToTexture( std::shared_ptr<LTexture> target, SDL_Texture *source, SDL_Rect *targetRect );

        // creates an empty LTexture of the specified dimensions with the desired LWindow
        std::shared_ptr<LTexture> createEmptyTexture( int width, int height, std::shared_ptr<LWindow> gHolder );
};




// creates an empty surface object with desired dimensions
SDL_Surface* createEmptySurface( int width, int height );