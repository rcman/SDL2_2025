#include "SDL.h"
#include "SDL_ttf.h"


int main(int argc, char *argv[]) {

    // Initialize SDL and SDL_ttf

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {

        SDL_Log("SDL_Init failed: %s", SDL_GetError());

        return 1;

    }

    if (TTF_Init() != 0) {

        SDL_Log("TTF_Init failed: %s", TTF_GetError());

        SDL_Quit();

        return 1;

    }


    // Create the main window

    SDL_Window *mainWindow = SDL_CreateWindow(

        "Main Window", 

        100, 100, 

        800, 600, 

        SDL_WINDOW_RESIZABLE

    );

    if (!mainWindow) {

        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());

        SDL_Quit();

        return 1;

    }


    // Create renderer for the main window

    SDL_Renderer *mainRenderer = SDL_CreateRenderer(mainWindow, NULL, 0);

    if (!mainRenderer) {

        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());

        SDL_DestroyWindow(mainWindow);

        SDL_Quit();

        return 1;

    }


    // Load font

    TTF_Font *font = TTF_OpenFont("path/to/font.ttf", 16); // Adjust path and size

    if (!font) {

        SDL_Log("TTF_OpenFont failed: %s", TTF_GetError());

        SDL_DestroyRenderer(mainRenderer);

        SDL_DestroyWindow(mainWindow);

        SDL_Quit();

        return 1;

    }


    SDL_Window *popupWindow = NULL;

    SDL_Renderer *popupRenderer = NULL;

    int running = 1;

    SDL_Event event;


    while (running) {

        while (SDL_PollEvent(&event)) {

            switch (event.type) {

            case SDL_EVENT_QUIT:

                running = 0;

                break;


            case SDL_EVENT_MOUSE_BUTTON_DOWN:

                if (event.button.button == SDL_BUTTON_LEFT) {

                    // Create the popup menu

                    if (!popupWindow) {

                        popupWindow = SDL_CreatePopupWindow(

                            mainWindow,

                            100, 50, // Position relative to main window

                            200, 100, // Size of the popup menu

                            SDL_WINDOW_BORDERLESS

                        );

                        popupRenderer = SDL_CreateRenderer(popupWindow, NULL, 0);

                    }

                }

                break;


            case SDL_EVENT_MOUSE_BUTTON_UP:

                // Destroy the popup menu when mouse button is released

                if (popupWindow) {

                    SDL_DestroyRenderer(popupRenderer);

                    SDL_DestroyWindow(popupWindow);

                    popupWindow = NULL;

                    popupRenderer = NULL;

                }

                break;

            }

        }


        // Clear main window

        SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 255);

        SDL_RenderClear(mainRenderer);


        // Render the popup menu if it exists

        if (popupRenderer) {

            SDL_SetRenderDrawColor(popupRenderer, 50, 50, 50, 255);

            SDL_RenderClear(popupRenderer);


            // Draw menu items

            SDL_SetRenderDrawColor(popupRenderer, 200, 200, 200, 255);

            SDL_Rect item1 = {10, 10, 180, 30};

            SDL_Rect item2 = {10, 50, 180, 30};

            SDL_RenderFillRect(popupRenderer, &item1);

            SDL_RenderFillRect(popupRenderer, &item2);


            // Render text on menu items

            SDL_Color textColor = {0, 0, 0, 255};

            SDL_Surface *textSurface1 = TTF_RenderText_Blended(font, "Option 1", textColor);

            SDL_Surface *textSurface2 = TTF_RenderText_Blended(font, "Option 2", textColor);

            SDL_Texture *textTexture1 = SDL_CreateTextureFromSurface(popupRenderer, textSurface1);

            SDL_Texture *textTexture2 = SDL_CreateTextureFromSurface(popupRenderer, textSurface2);


            SDL_Rect textRect1 = {20, 15, textSurface1->w, textSurface1->h};

            SDL_Rect textRect2 = {20, 55, textSurface2->w, textSurface2->h};


            SDL_RenderCopy(popupRenderer, textTexture1, NULL, &textRect1);

            SDL_RenderCopy(popupRenderer, textTexture2, NULL, &textRect2);


            // Clean up surfaces and textures

            SDL_FreeSurface(textSurface1);

            SDL_FreeSurface(textSurface2);

            SDL_DestroyTexture(textTexture1);

            SDL_DestroyTexture(textTexture2);


            SDL_RenderPresent(popupRenderer);

        }


        SDL_RenderPresent(mainRenderer);

        SDL_Delay(16); // ~60 FPS

    }


    // Cleanup

    if (popupRenderer) SDL_DestroyRenderer(popupRenderer);

    if (popupWindow) SDL_DestroyWindow(popupWindow);

    TTF_CloseFont(font);

    SDL_DestroyRenderer(mainRenderer);

    SDL_DestroyWindow(mainWindow);

    TTF_Quit();

    SDL_Quit();


    return 0;

}
