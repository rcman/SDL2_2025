#include <SDL3/SDL.h>
#include <stdbool.h>
#include <string.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MENU_HEIGHT 30

typedef struct {
    SDL_Rect rect;
    const char *label;
    bool selected;
} MenuItem;

void draw_menu(SDL_Renderer *renderer, MenuItem *menu, int menu_count) {
    // Draw menu background
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Dark gray
    SDL_RenderFillRect(renderer, &(SDL_Rect){0, 0, WINDOW_WIDTH, MENU_HEIGHT});

    // Draw menu items
    for (int i = 0; i < menu_count; i++) {
        SDL_SetRenderDrawColor(renderer, menu[i].selected ? 100 : 200, 200, 200, 255);
        SDL_RenderFillRect(renderer, &menu[i].rect);

        // Dummy label rendering - just colored rectangles for now
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black for text
        SDL_RenderDrawRect(renderer, &menu[i].rect); // Simulate border
    }
}

int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Create SDL window and renderer
    SDL_Window *window = SDL_CreateWindow("SDL3 Menu Example", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL, 0);

    if (!window || !renderer) {
        SDL_Log("Failed to create window or renderer: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Define menu items
    MenuItem menu[] = {
        {{10, 5, 60, MENU_HEIGHT - 10}, "File", false},
        {{80, 5, 60, MENU_HEIGHT - 10}, "Edit", false},
        {{150, 5, 60, MENU_HEIGHT - 10}, "Help", false}
    };
    int menu_count = sizeof(menu) / sizeof(menu[0]);

    // Main loop
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_MOUSE_MOTION:
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    for (int i = 0; i < menu_count; i++) {
                        if (SDL_PointInRect(&(SDL_Point){event.motion.x, event.motion.y}, &menu[i].rect)) {
                            menu[i].selected = true;
                            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                                SDL_Log("Clicked on menu: %s", menu[i].label);
                            }
                        } else {
                            menu[i].selected = false;
                        }
                    }
                    break;
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Draw menu
        draw_menu(renderer, menu, menu_count);

        // Present renderer
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

