#include <SDL2/SDL.h>
#include <vector>
#include <cmath>
#include <cstdlib>

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Water parameters
const int WATER_HEIGHT = 100;
const float WAVE_SPEED = 0.02f;

// Generate Perlin-like noise
float generateNoise(int x, int y, int seed) {
    x += seed;
    y += seed;
    int n = x + y * 57;
    n = (n << 13) ^ n;
    return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

void renderWater(SDL_Renderer* renderer, int screenWidth, int screenHeight, float time) {
    int seed = static_cast<int>(time * 10); // Change seed over time for animation

    // Set water color
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

    // Render water
    for (int x = 0; x < screenWidth; ++x) {
        float noise = generateNoise(x, seed, 0) * 20.0f; // Modulate amplitude
        int yStart = screenHeight - WATER_HEIGHT + static_cast<int>(noise);

        SDL_RenderDrawLine(renderer, x, yStart, x, screenHeight);
    }
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Realistic Water Effect", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Main loop
    bool running = true;
    SDL_Event event;
    float time = 0.0f;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render water
        renderWater(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, time);

        // Present the renderer
        SDL_RenderPresent(renderer);

        // Update time
        time += 0.1f;

        SDL_Delay(16); // Cap at ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
