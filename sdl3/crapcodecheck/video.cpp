#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h> 
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL initialization failed: %s", SDL_GetError());
        return 1;
    }

    SDL_DisplayID display = SDL_GetPrimaryDisplay();
    int num_modes = 0;
    SDL_DisplayMode **modes = SDL_GetFullscreenDisplayModes(display, &num_modes);

    if (modes) {
        printf("Available Fullscreen Modes for Display %" SDL_PRIu32 ":\n", display);
        for (int i = 0; i < num_modes; ++i) {
            SDL_DisplayMode *mode = modes[i];
            printf("Mode %d: %dx%d @ %.2fHz (Density: %.2f)\n",
                   i, mode->w, mode->h, mode->refresh_rate, mode->pixel_density);
        }
        SDL_free(modes);
    } else {
        SDL_Log("Failed to get fullscreen display modes: %s", SDL_GetError());
    }


    // Example of setting a window to fullscreen (after the user selects a mode, for instance)
    SDL_Window *window = SDL_CreateWindow("Fullscreen Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0); // Initial windowed
    if (!window) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Let's assume the user selected mode 2 (you would normally get this from user input)
    if (num_modes > 2) { //Make sure the mode exists.

        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP, modes[2]); // Use DESKTOP for borderless fullscreen
         SDL_Log("Set fullscreen to mode 2: %dx%d @ %.2fHz", modes[2]->w, modes[2]->h, modes[2]->refresh_rate);

        // Keep the window open for a bit (in a real application, you'd have a game loop, etc.)
        SDL_Delay(5000);

        //Switch back to windowed mode
        SDL_SetWindowFullscreen(window, 0, NULL); // 0 for windowed

    } else {
        SDL_Log("Mode 2 was not available.");
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
