#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTALSHAPE 10
#define TOTALANIMS 10
#define TOTALOBJECTS 20

struct fshape {
    int flag;
    unsigned char shp[272]; // Placeholder for shape data
};

struct animshape {
    int active;
    int animwidth;
    int animheight;
    int animox;
    int animoy;
    int animx;
    int animy;
    int prox;
    int animspeed;
    int currentshape;
    int oldshape;
    int max;
    int row;
    struct fshape *fshp[TOTALSHAPE];
} animobjects[TOTALANIMS];

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
int currentShape = 0;

void cleanup() {
    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

int initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    window = SDL_CreateWindow("SDL2 Animation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    return 1;
}

void drawShape(int rw) {
    int i, j;
    for (int sp = 0; sp < TOTALSHAPE; sp++) {
        for (i = 0; i < 16; i++) {
            for (j = 0; j < 16; j++) {
                if (animobjects[rw].fshp[sp]->shp[i * 16 + j]) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderDrawPoint(renderer, j + sp * 20, i);
                }
            }
        }
    }
}

void saveSpriteSheet() {
    SDL_Surface *surface = SDL_CreateRGBSurface(0, TOTALSHAPE * 20, 16, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    if (!surface) {
        printf("Failed to create surface: %s\n", SDL_GetError());
        return;
    }
    SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);
    IMG_SavePNG(surface, "spritesheet.png");
    SDL_FreeSurface(surface);
}

int fileRead(const char *filename) {
    FILE *in = fopen(filename, "rb");
    if (!in) {
        fprintf(stderr, "Cannot open input file.\n");
        return 0;
    }
    for (int numrow = 0; numrow < TOTALANIMS; numrow++) {
        fread(&animobjects[numrow].max, sizeof(int), 1, in);
        for (int numobj = 0; numobj <= animobjects[numrow].max; numobj++) {
            fread(animobjects[numrow].fshp[numobj], 272, 1, in);
            animobjects[numrow].fshp[numobj]->flag = 1;
        }
    }
    fclose(in);
    return 1;
}

int main(int argc, char *argv[]) {
    if (!initSDL()) {
        return -1;
    }
    int running = 1;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = 0;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        if (currentShape > 0) currentShape--;
                        break;
                    case SDLK_RIGHT:
                        if (currentShape < TOTALSHAPE - 1) currentShape++;
                        break;
                    case SDLK_s:
                        saveSpriteSheet();
                        break;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        drawShape(currentShape);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    cleanup();
    return 0;
}
