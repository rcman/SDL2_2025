#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

// Function to check if a pixel color matches the background color
// Parameters:
// r, g, b - Color values of the current pixel
// br, bg, bb - Background color values to compare against
bool isBackgroundColor(Uint8 r, Uint8 g, Uint8 b, Uint8 br, Uint8 bg, Uint8 bb) {
    // Compare pixel color to background color
    return (r == br && g == bg && b == bb);
}

int main(int argc, char* argv[]) {
    // Initialize SDL with video support
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize SDL_image with PNG support
    if (!IMG_Init(IMG_INIT_PNG)) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Load the image file
    SDL_Surface* image = IMG_Load("/mnt/data/unnamed-2.jpg");
    if (!image) {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Define the background color (white in this case)
    Uint32 backgroundColor = SDL_MapRGB(image->format, 255, 255, 255); // Assuming white background
    Uint8 bgR, bgG, bgB;
    SDL_GetRGB(backgroundColor, image->format, &bgR, &bgG, &bgB);

    // Create an output surface with an alpha channel for transparency
    SDL_Surface* output = SDL_CreateRGBSurfaceWithFormat(0, image->w, image->h, 32, SDL_PIXELFORMAT_RGBA32);
    if (!output) {
        std::cerr << "Failed to create output surface: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(image);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Access pixel data of the input and output surfaces
    Uint32* pixels = (Uint32*)image->pixels;
    Uint32* outPixels = (Uint32*)output->pixels;

    // Iterate through each pixel of the image
    for (int y = 0; y < image->h; ++y) {
        for (int x = 0; x < image->w; ++x) {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[y * image->w + x], image->format, &r, &g, &b);

            // Check if the pixel matches the background color
            if (isBackgroundColor(r, g, b, bgR, bgG, bgB)) {
                outPixels[y * image->w + x] = SDL_MapRGBA(output->format, 0, 0, 0, 0); // Transparent
            } else {
                outPixels[y * image->w + x] = SDL_MapRGBA(output->format, r, g, b, 255); // Opaque
            }
        }
    }

    // Save the output surface as a PNG file
    if (IMG_SavePNG(output, "output.png") != 0) {
        std::cerr << "Failed to save output image: " << IMG_GetError() << std::endl;
    } else {
        std::cout << "Image saved as output.png" << std::endl;
    }

    // Free allocated resources
    SDL_FreeSurface(output);
    SDL_FreeSurface(image);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
