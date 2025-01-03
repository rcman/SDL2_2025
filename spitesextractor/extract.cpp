#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <string>

// Function to save SDL_Texture to a PNG file
bool SaveTextureAsPNG(SDL_Renderer* renderer, SDL_Texture* texture, const std::string& fileName) {
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, 0, 0, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        std::cerr << "Failed to create surface: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_SetRenderTarget(renderer, texture);
    if (SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_RGBA32, surface->pixels, surface->pitch) != 0) {
        std::cerr << "Failed to read pixels: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return false;
    }

    if (IMG_SavePNG(surface, fileName.c_str()) != 0) {
        std::cerr << "Failed to save PNG: " << IMG_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return false;
    }

    SDL_FreeSurface(surface);
    return true;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Sprite Extraction", 100, 100, 800, 600, SDL_WINDOW_HIDDEN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* spriteSheet = IMG_LoadTexture(renderer, "spritesheet.png");
    if (!spriteSheet) {
        std::cerr << "IMG_LoadTexture Error: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int sheetWidth, sheetHeight;
    SDL_QueryTexture(spriteSheet, nullptr, nullptr, &sheetWidth, &sheetHeight);

    const int spriteSize = 32;
    int spritesPerRow = sheetWidth / spriteSize;
    int spritesPerCol = sheetHeight / spriteSize;
    int totalSprites = spritesPerRow * spritesPerCol;

    SDL_Texture* outputTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, spriteSize * totalSprites, spriteSize);
    if (!outputTexture) {
        std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
        SDL_DestroyTexture(spriteSheet);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetTextureBlendMode(outputTexture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, outputTexture);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    for (int i = 0; i < totalSprites; ++i) {
        int srcX = (i % spritesPerRow) * spriteSize;
        int srcY = (i / spritesPerRow) * spriteSize;
        SDL_Rect srcRect = { srcX, srcY, spriteSize, spriteSize };

        SDL_Rect dstRect = { i * spriteSize, 0, spriteSize, spriteSize };
        SDL_RenderCopy(renderer, spriteSheet, &srcRect, &dstRect);
    }

    SDL_RenderPresent(renderer);

    if (!SaveTextureAsPNG(renderer, outputTexture, "output.png")) {
        std::cerr << "Failed to save output texture." << std::endl;
    }

    SDL_DestroyTexture(outputTexture);
    SDL_DestroyTexture(spriteSheet);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
