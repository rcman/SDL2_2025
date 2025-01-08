#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>

// Joint structure
struct Joint {
    int x, y;
    std::string name;
};

// Main function
int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || IMG_Init(IMG_INIT_PNG) < 0) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Animation Tool", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load image
    SDL_Surface* imageSurface = IMG_Load("character.png");
    if (!imageSurface) {
        SDL_Log("Failed to load image: %s", IMG_GetError());
        return -1;
    }
    SDL_Texture* characterTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);

    bool running = true;
    SDL_Event event;
    std::vector<Joint> joints;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                // Add joint on mouse click
                joints.push_back({event.button.x, event.button.y, "Joint" + std::to_string(joints.size() + 1)});
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render character
        SDL_RenderCopy(renderer, characterTexture, nullptr, nullptr);

        // Render joints
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (const auto& joint : joints) {
            SDL_Rect rect = {joint.x - 5, joint.y - 5, 10, 10};
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(characterTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
