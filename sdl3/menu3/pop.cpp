#include <SDL3/SDL.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create the main window
    SDL_Window* main_window = SDL_CreateWindow(
        "Main Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_RESIZABLE
    );

    if (!main_window) {
        printf("Main window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a popup window relative to the main window
    SDL_Window* popup_window = SDL_CreatePopupWindow(
        main_window,
        100, // x position relative to the main window
        100, // y position relative to the main window
        300, // width
        200, // height
        SDL_WINDOW_TOOLTIP // Example flag for popup window
    );

    if (!popup_window) {
        printf("Popup window creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(main_window);
        SDL_Quit();
        return 1;
    }

    // Main event loop
    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }
        }

        SDL_Delay(16); // Simulate 60 FPS
    }

    // Clean up
    SDL_DestroyWindow(popup_window);
    SDL_DestroyWindow(main_window);
    SDL_Quit();

    return 0;
}

