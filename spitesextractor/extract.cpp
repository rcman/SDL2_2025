#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>

// Function to save an SDL_Surface as a PNG
bool SaveSurfaceAsPNG(SDL_Surface* surface, const std::string& fileName) {
    if (IMG_SavePNG(surface, fileName.c_str()) != 0) {
        std::cerr << "Failed to save PNG: " << IMG_GetError() << std::endl;
        return false;
    }
    return true;
}

// Function to detect sprites with improved handling of long/large sprites
std::vector<SDL_Rect> DetectSprites(SDL_Surface* surface, Uint32 bgColor) {
    int width = surface->w;
    int height = surface->h;

    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
    std::vector<SDL_Rect> spriteRects;

    auto isBackground = [&](Uint32 color) {
        return color == bgColor;
    };

    auto getPixelColor = [&](int x, int y) -> Uint32 {
        Uint32* pixels = (Uint32*)surface->pixels;
        return pixels[y * surface->w + x];
    };

    auto floodFill = [&](int startX, int startY, SDL_Rect& rect) {
        std::queue<std::pair<int, int>> q;
        q.emplace(startX, startY);

        rect.x = startX;
        rect.y = startY;
        rect.w = 0;
        rect.h = 0;

        while (!q.empty()) {
            auto [x, y] = q.front();
            q.pop();

            if (x < 0 || x >= width || y < 0 || y >= height || visited[y][x]) continue;
            if (isBackground(getPixelColor(x, y))) continue;

            visited[y][x] = true;

            // Update the bounding box
            rect.x = std::min(rect.x, x);
            rect.y = std::min(rect.y, y);
            rect.w = std::max(rect.w, x - rect.x + 1);
            rect.h = std::max(rect.h, y - rect.y + 1);

            // Add neighbors to the queue
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    if (dx != 0 || dy != 0) {
                        q.emplace(x + dx, y + dy);
                    }
                }
            }
        }
    };

    // Traverse the image to find sprites
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (!visited[y][x] && !isBackground(getPixelColor(x, y))) {
                SDL_Rect rect = { x, y, 0, 0 };
                floodFill(x, y, rect);

                // Merge nearby bounding boxes (if overlapping or within margin)
                bool merged = false;
                for (auto& existingRect : spriteRects) {
                    int margin = 5; // Adjust this margin to handle gaps
                    if (rect.x < existingRect.x + existingRect.w + margin &&
                        rect.x + rect.w + margin > existingRect.x &&
                        rect.y < existingRect.y + existingRect.h + margin &&
                        rect.y + rect.h + margin > existingRect.y) {
                        existingRect.x = std::min(existingRect.x, rect.x);
                        existingRect.y = std::min(existingRect.y, rect.y);
                        existingRect.w = std::max(existingRect.x + existingRect.w, rect.x + rect.w) - existingRect.x;
                        existingRect.h = std::max(existingRect.y + existingRect.h, rect.y + rect.h) - existingRect.y;
                        merged = true;
                        break;
                    }
                }

                if (!merged) {
                    spriteRects.push_back(rect);
                }
            }
        }
    }

    return spriteRects;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Load the sprite sheet as a surface
    SDL_Surface* spriteSheet = IMG_Load("cars_pink_background.png");
    if (!spriteSheet) {
        std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Detect the background color (top-left pixel is assumed to be the background)
    Uint32 bgColor = *((Uint32*)spriteSheet->pixels);

    // Detect individual sprites
    std::vector<SDL_Rect> spriteRects = DetectSprites(spriteSheet, bgColor);
    std::cout << "Detected " << spriteRects.size() << " sprites." << std::endl;

    // Extract and save each sprite
    for (size_t i = 0; i < spriteRects.size(); ++i) {
        SDL_Rect rect = spriteRects[i];
        SDL_Surface* spriteSurface = SDL_CreateRGBSurfaceWithFormat(0, rect.w, rect.h, 32, spriteSheet->format->format);

        if (!spriteSurface) {
            std::cerr << "Failed to create sprite surface: " << SDL_GetError() << std::endl;
            continue;
        }

        SDL_BlitSurface(spriteSheet, &rect, spriteSurface, nullptr);

        std::string fileName = "sprite_" + std::to_string(i) + ".png";
        if (!SaveSurfaceAsPNG(spriteSurface, fileName)) {
            std::cerr << "Failed to save sprite: " << fileName << std::endl;
        } else {
            std::cout << "Saved: " << fileName << std::endl;
        }

        SDL_FreeSurface(spriteSurface);
    }

    SDL_FreeSurface(spriteSheet);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
