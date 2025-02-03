#include <SDL3/SDL.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int BLOCK_SIZE = 32; // Example block size

// Function to load the map from a file
std::vector<std::vector<int>> loadMap(const std::string& filename) {
    std::vector<std::vector<int>> mapData;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening map file: " << filename << std::endl;
        return mapData; // Return empty if file fails to open
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        std::stringstream ss(line);
        std::string value;

        while (std::getline(ss, value, ',')) {
            try {
                row.push_back(std::stoi(value));
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid number in map file: " << value << std::endl;
                // Handle the error, e.g., skip the invalid value
            } catch (const std::out_of_range& e) {
                std::cerr << "Number out of range in map file: " << value << std::endl;
            }
        }
        mapData.push_back(row);
    }
    file.close();
    return mapData;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Map Loader", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        return 1;
    }


    std::vector<std::vector<int>> map = loadMap("map.txt");

    // Example: Load block textures.  Replace these with your actual block loading logic
    std::vector<SDL_Texture*> blockTextures;
    blockTextures.push_back(nullptr); // Index 0 - no block (or you can use a default)
    blockTextures.push_back(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, BLOCK_SIZE, BLOCK_SIZE)); // Example texture
    blockTextures.push_back(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, BLOCK_SIZE, BLOCK_SIZE)); // Example texture
    // ... load other block textures

    if (blockTextures[1]) {
        Uint32 *pixels = new Uint32[BLOCK_SIZE * BLOCK_SIZE];
        for (int i = 0; i < BLOCK_SIZE * BLOCK_SIZE; i++) {
            pixels[i] = 0xFF0000; // Red
        }
        SDL_UpdateTexture(blockTextures[1], NULL, pixels, BLOCK_SIZE * 4);
        delete[] pixels;
    }
    if (blockTextures[2]) {
        Uint32 *pixels = new Uint32[BLOCK_SIZE * BLOCK_SIZE];
        for (int i = 0; i < BLOCK_SIZE * BLOCK_SIZE; i++) {
            pixels[i] = 0x00FF00; // Green
        }
        SDL_UpdateTexture(blockTextures[2], NULL, pixels, BLOCK_SIZE * 4);
        delete[] pixels;
    }


    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear to black
        SDL_RenderClear(renderer);

        // Draw the map
        for (size_t y = 0; y < map.size(); ++y) {
            for (size_t x = 0; x < map[y].size(); ++x) {
                int blockType = map[y][x];
                if (blockType > 0 && blockType < blockTextures.size() && blockTextures[blockType]) { // Check for valid block type and texture
                    SDL_Rect destRect = { static_cast<int>(x * BLOCK_SIZE), static_cast<int>(y * BLOCK_SIZE), BLOCK_SIZE, BLOCK_SIZE };
                    SDL_RenderCopy(renderer, blockTextures[blockType], nullptr, &destRect);
                }
            }
        }


        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Clean up textures
    for (SDL_Texture* texture : blockTextures) {
        SDL_DestroyTexture(texture);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
