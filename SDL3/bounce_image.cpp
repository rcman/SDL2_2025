#include <SDL3/SDL.h>
#include <iostream>

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Bouncing Image", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Load the image (replace "image.png" with your image file)
    SDL_Surface* imageSurface = SDL_LoadBMP("image.bmp"); // or PNG, etc.
    if (!imageSurface) {
        std::cerr << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Texture* imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface); // Free the surface after creating the texture

    if (!imageTexture) {
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    int imageWidth = 128;  // Image dimensions
    int imageHeight = 128;

    int x = 100;          // Initial position
    int y = 50;
    int dx = 5;           // Speed in x and y directions
    int dy = 5;

    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        // Update image position
        x += dx;
        y += dy;

        // Bounce off the screen edges
        int windowWidth, windowHeight;
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);

        if (x < 0 || x + imageWidth > windowWidth) {
            dx = -dx;
        }
        if (y < 0 || y + imageHeight > windowHeight) {
            dy = -dy;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear to black
        SDL_RenderClear(renderer);

        SDL_Rect destRect = { x, y, imageWidth, imageHeight };
        SDL_RenderCopy(renderer, imageTexture, nullptr, &destRect);

        SDL_RenderPresent(renderer);

        SDL_Delay(16); // Cap frame rate (approximately 60 FPS)
    }

    SDL_DestroyTexture(imageTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
