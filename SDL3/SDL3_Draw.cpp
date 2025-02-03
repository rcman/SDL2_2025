#include <SDL3/SDL.h>
#include <stdbool.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define PALETTE_SIZE 5

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *canvasTexture;
SDL_Rect palette[PALETTE_SIZE];
SDL_Color colors[PALETTE_SIZE] = {
    {255, 0, 0, 255},  // Red
    {0, 255, 0, 255},  // Green
    {0, 0, 255, 255},  // Blue
    {0, 0, 0, 255},    // Black
    {255, 255, 255, 255} // White
};
SDL_Color selectedColor = {0, 0, 0, 255};
SDL_bool isDrawing = SDL_FALSE;
SDL_Rect selection = {0, 0, 0, 0};
SDL_Texture *clipboard = NULL;

void DrawPalette() {
    for (int i = 0; i < PALETTE_SIZE; i++) {
        SDL_SetRenderDrawColor(renderer, colors[i].r, colors[i].g, colors[i].b, 255);
        SDL_RenderFillRect(renderer, &palette[i]);
    }
}

void HandleMouseDown(SDL_MouseButtonEvent *event) {
    if (event->button == SDL_BUTTON_LEFT) {
        for (int i = 0; i < PALETTE_SIZE; i++) {
            if (SDL_PointInRect(&(SDL_Point){event->x, event->y}, &palette[i])) {
                selectedColor = colors[i];
                return;
            }
        }
        isDrawing = SDL_TRUE;
    }
}

void HandleMouseUp(SDL_MouseButtonEvent *event) {
    if (event->button == SDL_BUTTON_LEFT) {
        isDrawing = SDL_FALSE;
    }
}

void HandleMouseMotion(SDL_MouseMotionEvent *event) {
    if (isDrawing) {
        SDL_SetRenderTarget(renderer, canvasTexture);
        SDL_SetRenderDrawColor(renderer, selectedColor.r, selectedColor.g, selectedColor.b, 255);
        SDL_RenderDrawPoint(renderer, event->x, event->y);
        SDL_SetRenderTarget(renderer, NULL);
    }
}

void CopySelection() {
    if (selection.w > 0 && selection.h > 0) {
        clipboard = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, selection.w, selection.h);
        SDL_SetRenderTarget(renderer, clipboard);
        SDL_RenderCopy(renderer, canvasTexture, &selection, NULL);
        SDL_SetRenderTarget(renderer, NULL);
    }
}

void PasteSelection(int x, int y) {
    if (clipboard) {
        SDL_Rect dst = {x, y, selection.w, selection.h};
        SDL_SetRenderTarget(renderer, canvasTexture);
        SDL_RenderCopy(renderer, clipboard, NULL, &dst);
        SDL_SetRenderTarget(renderer, NULL);
    }
}

void HandleKeyboard(SDL_KeyboardEvent *event) {
    if (event->type == SDL_EVENT_KEY_DOWN) {
        if (event->keysym.sym == SDLK_c) {
            CopySelection();
        } else if (event->keysym.sym == SDLK_v) {
            PasteSelection(100, 100); // Example fixed position
        }
    }
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("SDL3 Drawing App", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_ACCELERATED);
    canvasTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    for (int i = 0; i < PALETTE_SIZE; i++) {
        palette[i] = (SDL_Rect){10 + i * 40, 10, 30, 30};
    }
    
    SDL_SetRenderTarget(renderer, canvasTexture);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, NULL);
    
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                HandleMouseDown(&event.button);
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                HandleMouseUp(&event.button);
            } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                HandleMouseMotion(&event.motion);
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                HandleKeyboard(&event.key);
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, canvasTexture, NULL, NULL);
        DrawPalette();
        SDL_RenderPresent(renderer);
    }
    
    SDL_DestroyTexture(canvasTexture);
    if (clipboard) SDL_DestroyTexture(clipboard);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
