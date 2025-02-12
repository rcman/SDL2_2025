#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <numeric> // For std::accumulate

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Surface* spriteSheet = IMG_Load("spritesheet.png"); // Replace with your sprite sheet path
    if (!spriteSheet) {
        std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    int spriteSheetWidth = spriteSheet->w;
    int spriteSheetHeight = spriteSheet->h;

    int numSprites = 8; // Assuming 8 sprites

    std::vector<int> spriteWidths;
    std::vector<int> spriteHeights;

    // *** KEY CHANGE: Logic to determine individual sprite widths and heights ***
    // This example assumes sprites are arranged horizontally in a grid.  
    // You'll likely need to adapt this part based on *your* sprite sheet layout.
    for (int i = 0; i < numSprites; ++i) {
        // Example: Assuming equal spacing.  Adjust as needed for your sheet.
        int xStart = (spriteSheetWidth / numSprites) * i;
        int width = spriteSheetWidth / numSprites; // Or some other logic based on your sheet
        int height = spriteSheetHeight; // Or some other logic based on your sheet
        spriteWidths.push_back(width);
        spriteHeights.push_back(height);
    }



    // Calculate the average sprite width and height
    double averageWidth = std::accumulate(spriteWidths.begin(), spriteWidths.end(), 0.0) / spriteWidths.size();
    double averageHeight = std::accumulate(spriteHeights.begin(), spriteHeights.end(), 0.0) / spriteHeights.size();


    // Create a new surface for the resized sprite sheet
    SDL_Surface* resizedSheet = SDL_CreateRGBSurface(0, static_cast<int>(averageWidth) * numSprites, static_cast<int>(averageHeight), 32,
                                                     0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000); // Use averageWidth and averageHeight

    if (!resizedSheet) {
        std::cerr << "SDL_CreateRGBSurface Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(spriteSheet);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Resize and copy each sprite
    for (int i = 0; i < numSprites; ++i) {
        SDL_Rect srcRect;
        srcRect.x = (spriteSheetWidth / numSprites) * i; // Start at the beginning of each sprite section
        srcRect.y = 0;
        srcRect.w = spriteWidths[i]; // Use the individual sprite widths
        srcRect.h = spriteHeights[i]; // Use individual sprite heights

        SDL_Rect destRect;
        destRect.x = static_cast<int>(i * averageWidth); // Position in the new sheet
        destRect.y = 0;
        destRect.w = static_cast<int>(averageWidth); // Use the average width
        destRect.h = static_cast<int>(averageHeight); // Use the average height

        // Use SDL_BlitScaled to resize the sprite during the copy
        if (SDL_BlitScaled(spriteSheet, &srcRect, resizedSheet, &destRect) < 0) {
            std::cerr << "SDL_BlitScaled Error: " << SDL_GetError() << std::endl;
            SDL_FreeSurface(spriteSheet);
            SDL_FreeSurface(resizedSheet);
            IMG_Quit();
            SDL_Quit();
            return 1;
        }
    }

    // Save the resized sprite sheet
    if (IMG_SavePNG(resizedSheet, "resized_spritesheet.png") < 0) { // Save as PNG
        std::cerr << "IMG_SavePNG Error: " << IMG_GetError() << std::endl;
    } else {
        std::cout << "Resized sprite sheet saved to resized_spritesheet.png" << std::endl;
    }


    SDL_FreeSurface(spriteSheet);
    SDL_FreeSurface(resizedSheet);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
