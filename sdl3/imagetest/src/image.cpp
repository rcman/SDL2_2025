#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>

// Define window dimensions
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

int main(int argc, char *argv) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window *window = SDL_CreateWindow("SDL Image Example",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_RESIZABLE);
    if (!window) {
        fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Load the image
    SDL_Surface *imageSurface = SDL_LoadBMP("image.bmp"); // Replace with your image path
    if (!imageSurface) {
        fprintf(stderr, "Error loading image: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Create a texture from the image
    SDL_Texture *imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_DestroySurface(imageSurface); // Free the surface, as the texture has the image data
    if (!imageTexture) {
        fprintf(stderr, "Error creating texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Get image dimensions
    int imageWidth, imageHeight;
    SDL_QueryTexture(imageTexture, NULL, NULL, &imageWidth, &imageHeight);

    // Create a rectangle to specify the destination for the image
    SDL_Rect destRect = {
        (WINDOW_WIDTH - imageWidth) / 2, // X position (centered)
        (WINDOW_HEIGHT - imageHeight) / 2, // Y position (centered)
        imageWidth,          // Width
        imageHeight          // Height
    };

    // Main loop
    bool quit = false;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        // Clear the renderer
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set background color (black)
        SDL_RenderClear(renderer);

        // Render the image (copy the texture to the renderer)
        SDL_RenderTexture(renderer, imageTexture, NULL, &destRect);

        // Present the rendered image on the screen
        SDL_RenderPresent(renderer);
    }

    // Clean up
    SDL_DestroyTexture(imageTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
