#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

// Define the initial values
int health = 100;
int ammo = 30;
int grenades = 5;
int shieldStrength = 1; // Shield starts at 1, max is 7

// Function to render text
void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color, int x, int y) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dstRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Function to render shield boxes
void renderShieldBoxes(SDL_Renderer* renderer, int shieldStrength, SDL_Color boxColor, int x, int y, int boxWidth, int boxHeight, int spacing) {
    // Draw filled boxes for the current shield strength
    for (int i = 0; i < shieldStrength; ++i) {
        SDL_Rect box = {x + i * (boxWidth + spacing), y, boxWidth, boxHeight};
        SDL_SetRenderDrawColor(renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
        SDL_RenderFillRect(renderer, &box);
    }

    // Draw empty boxes for the remaining slots
    SDL_Color emptyBoxColor = {100, 100, 100, 255}; // Grey for empty boxes
    for (int i = shieldStrength; i < 7; ++i) {
        SDL_Rect box = {x + i * (boxWidth + spacing), y, boxWidth, boxHeight};
        SDL_SetRenderDrawColor(renderer, emptyBoxColor.r, emptyBoxColor.g, emptyBoxColor.b, emptyBoxColor.a);
        SDL_RenderDrawRect(renderer, &box); // Draw an empty box
    }
}

int main(int argc, char* argv[]) {
    // Initialize SDL and SDL_ttf
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0) {
        std::cerr << "Failed to initialize SDL or SDL_ttf: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create a window and renderer
    SDL_Window* window = SDL_CreateWindow("HUD Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load font
    TTF_Font* font = TTF_OpenFont("path_to_font.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return -1;
    }

    // Define colors
    SDL_Color healthColor = {255, 0, 0, 255};       // Red
    SDL_Color ammoColor = {0, 255, 0, 255};         // Green
    SDL_Color grenadesColor = {255, 255, 0, 255};   // Yellow
    SDL_Color shieldBoxColor = {0, 0, 255, 255};    // Blue

    // Game loop
    bool running = true;
    SDL_Event event;

    while (running) {
        // Event handling
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            // Simulate shield power-up on pressing the 'P' key
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p) {
                if (shieldStrength < 7) {
                    shieldStrength++; // Increase shield strength
                }
            }
            // Simulate shield damage on pressing the 'D' key
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_d) {
                if (shieldStrength > 0) {
                    shieldStrength--; // Decrease shield strength
                }
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        // Render HUD elements
        renderText(renderer, font, "Health: " + std::to_string(health), healthColor, 50, 50);
        renderText(renderer, font, "Ammo: " + std::to_string(ammo), ammoColor, 50, 100);
        renderText(renderer, font, "Grenades: " + std::to_string(grenades), grenadesColor, 50, 150);

        // Render shield boxes
        renderShieldBoxes(renderer, shieldStrength, shieldBoxColor, 50, 200, 30, 30, 10);

        // Present the renderer
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // Simulate ~60 FPS
    }

    // Cleanup
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
