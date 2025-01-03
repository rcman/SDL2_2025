#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
// This code dynamically detects regions and extracts vehicles of varying sizes. 
// Helper function to check if a pixel is part of a vehicle (e.g., based on color or brightness)
bool isVehiclePixel(Uint32 pixel, SDL_PixelFormat* format) {
    Uint8 r, g, b, a;
    SDL_GetRGBA(pixel, format, &r, &g, &b, &a);
    // Replace with a condition based on your images
    return (r > 50 && g < 50 && b < 50);  // Example: red-dominant pixels
}

// Extract a bounding box from connected pixels
void detectRegions(SDL_Surface* source, SDL_Rect* regions, int* regionCount, int maxRegions) {
    int width = source->w;
    int height = source->h;
    Uint32* pixels = (Uint32*)source->pixels;
    Uint8* visited = calloc(width * height, sizeof(Uint8)); // Keep track of visited pixels

    *regionCount = 0;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (visited[y * width + x]) continue; // Skip visited pixels
            Uint32 pixel = pixels[y * width + x];
            if (!isVehiclePixel(pixel, source->format)) continue; // Skip non-vehicle pixels

            // Find bounding box for the connected region
            int minX = x, maxX = x, minY = y, maxY = y;

            // Flood-fill to find connected pixels
            for (int fy = minY; fy <= maxY; ++fy) {
                for (int fx = minX; fx <= maxX; ++fx) {
                    if (fx < 0 || fy < 0 || fx >= width || fy >= height) continue;
                    if (visited[fy * width + fx]) continue;
                    if (isVehiclePixel(pixels[fy * width + fx], source->format)) {
                        visited[fy * width + fx] = 1;
                        minX = fx < minX ? fx : minX;
                        maxX = fx > maxX ? fx : maxX;
                        minY = fy < minY ? fy : minY;
                        maxY = fy > maxY ? fy : maxY;
                    }
                }
            }

            // Add region if valid
            if (*regionCount < maxRegions) {
                regions[*regionCount] = (SDL_Rect){minX, minY, maxX - minX + 1, maxY - minY + 1};
                (*regionCount)++;
            }
        }
    }

    free(visited);
}

// Main function
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input_image>\n", argv[0]);
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0 || IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        printf("Initialization Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface* source = IMG_Load(argv[1]);
    if (!source) {
        printf("Failed to load image: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Rect regions[100];
    int regionCount = 0;
    detectRegions(source, regions, &regionCount, 100);

    for (int i = 0; i < regionCount; ++i) {
        SDL_Rect region = regions[i];
        SDL_Surface* cropped = SDL_CreateRGBSurface(0, region.w, region.h, source->format->BitsPerPixel,
                                                     source->format->Rmask, source->format->Gmask,
                                                     source->format->Bmask, source->format->Amask);

        SDL_BlitSurface(source, &region, cropped, NULL);

        char filename[128];
        snprintf(filename, sizeof(filename), "vehicle_%03d.png", i);
        if (IMG_SavePNG(cropped, filename) != 0) {
            printf("Failed to save image %s: %s\n", filename, IMG_GetError());
        }

        SDL_FreeSurface(cropped);
    }

    SDL_FreeSurface(source);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
