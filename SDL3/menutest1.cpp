#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <string>

// Structure to represent a menu item
struct MenuItem {
    std::string text;
    // Add a function pointer or ID to handle the action when selected.
    // For simplicity, we'll just print to console here.
    int id; // Unique identifier for the menu item
};

// Function to draw the menu
void drawMenu(SDL_Renderer* renderer, const std::vector<MenuItem>& menuItems, int selectedItem) {
    int y = 50; // Starting Y position for the menu
    int itemHeight = 30; // Height of each menu item
    int itemWidth = 200; // Width of each menu item

    for (size_t i = 0; i < menuItems.size(); ++i) {
        SDL_Rect rect = { 50, y, itemWidth, itemHeight };

        // Highlight the selected item
        if (static_cast<int>(i) == selectedItem) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue for selected
        } else {
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Gray for others
        }
        SDL_RenderFillRect(renderer, &rect);

        // Draw the text
        SDL_Color textColor = { 255, 255, 255, 255 }; // White
        SDL_Surface* textSurface = SDL_CreateRGBSurfaceWithFormat(0,0,0,32,SDL_PIXELFORMAT_RGBA8888);
        SDL_RWops* rw = SDL_RWFromFile("font.ttf", "rb"); // Replace "font.ttf" with your font file.  Make sure it is in the same directory as your executable.
        TTF_Font* font = TTF_OpenFontRW(rw, 24);
        if (!font) {
            std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
            return; // Handle font loading error appropriately
        }
        SDL_Surface* textSurf = TTF_RenderText_Solid(font, menuItems[i].text.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurf);
        SDL_Rect textRect = { rect.x + 10, rect.y + 5, textSurf->w, textSurf->h };
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

        SDL_DestroyTexture(textTexture);
        SDL_FreeSurface(textSurf);
        TTF_CloseFont(font);
        SDL_RWclose(rw);

        y += itemHeight + 10; // Spacing between items
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() < 0) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("SDL3 Menu Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    std::vector<MenuItem> menuItems = {
        {"Start Game", 1},
        {"Options", 2},
        {"Quit", 3}
    };

    int selectedItem = 0;

    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            } else if (event.type == SDL_EVENT_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        selectedItem = (selectedItem - 1 + menuItems.size()) % menuItems.size();
                        break;
                    case SDLK_DOWN:
                        selectedItem = (selectedItem + 1) % menuItems.size();
                        break;
                    case SDLK_RETURN: // Enter key
                        // Handle menu item selection
                        std::cout << "Selected: " << menuItems[selectedItem].text << std::endl;
                        if (menuItems[selectedItem].id == 3) { // Example: Quit
                            quit = true;
                        }
                        break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        drawMenu(renderer, menuItems, selectedItem);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
