#include "TextureManipulator.hpp"




TextureManipulator::TextureManipulator() {}


void TextureManipulator::renderTextureToTexture( std::shared_ptr<LTexture> target, std::shared_ptr<LTexture> source, SDL_Rect *targetRect )
{
    // Ensure both target and source are valid
    if (!target || !source || !target->mTexture || !source->mTexture) {
        std::cerr << "Invalid target or source texture." << std::endl;
        return;
    }

    SDL_Renderer *renderer = source->gHolder->gRenderer;

    // Set the source's renderer to render onto the target texture
    if (SDL_SetRenderTarget(renderer, target->mTexture) != 0) {
        std::cerr << "SDL_SetRenderTarget failed: " << SDL_GetError() << std::endl;
        return;
    }

    // Copy the source texture onto the target texture
    if (SDL_RenderCopy(renderer, source->mTexture, NULL, targetRect) != 0) {
        std::cerr << "SDL_RenderCopy failed: " << SDL_GetError() << std::endl;
    }

    // Reset the target to the default render target
    if (SDL_SetRenderTarget(renderer, NULL) != 0) {
        std::cerr << "SDL_SetRenderTarget failed: " << SDL_GetError() << std::endl;
    }
}



void TextureManipulator::renderTextureToTexture( std::shared_ptr<LTexture> target, SDL_Texture *source, SDL_Rect *targetRect )
{
    // Ensure both target and source are valid
    if (!target || !source || !target->mTexture) {
        std::cerr << "Invalid target or source texture." << std::endl;
        return;
    }

    SDL_Renderer *renderer = target->gHolder->gRenderer;

    // Set the source's renderer to render onto the target texture
    if (SDL_SetRenderTarget(renderer, target->mTexture) != 0) {
        std::cerr << "SDL_SetRenderTarget failed: " << SDL_GetError() << std::endl;
        return;
    }

    // Copy the source texture onto the target texture
    if (SDL_RenderCopy(renderer, source, NULL, targetRect) != 0) {
        std::cerr << "SDL_RenderCopy failed: " << SDL_GetError() << std::endl;
    }

    // Reset the target to the default render target
    if (SDL_SetRenderTarget(renderer, NULL) != 0) {
        std::cerr << "SDL_SetRenderTarget failed: " << SDL_GetError() << std::endl;
    }
}




// creates an empty LTexture of the specified dimensions with the desired LWindow
std::shared_ptr<LTexture> TextureManipulator::createEmptyTexture( int width, int height, std::shared_ptr<LWindow> gHolder )
{
    // create an LTexture pointer
    auto res = std::make_shared<LTexture>(gHolder);

    SDL_Texture *newTexture = NULL;
    newTexture = SDL_CreateTexture(gHolder->gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

    // set the mTexture of res
    res->mTexture = newTexture;
    res->mWidth = width; res->mHeight = height;
    return res;
}







SDL_Surface* createEmptySurface( int width, int height )
{
    // Create an empty surface with the specified width, height, and 32-bit color depth
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32,
                                                0x00FF0000, // Red mask
                                                0x0000FF00, // Green mask
                                                0x000000FF, // Blue mask
                                                0xFF000000  // Alpha mask
                                                );
    if (!surface) std::cerr << "SDL_CreateRGBSurface failed: "<<SDL_GetError() <<std::endl;

    return surface;
}