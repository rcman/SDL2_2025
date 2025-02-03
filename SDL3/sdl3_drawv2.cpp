#include <SDL3/SDL.h>
#include <iostream>
#include <vector>

// Structure to represent a point
struct Point {
    int x;
    int y;
};

// Structure to represent a line
struct Line {
    Point start;
    Point end;
    SDL_Color color;
};

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Simple Drawing Program", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::vector<Line> lines;
    Point currentPoint = {0, 0};
    bool isDrawing = false;
    SDL_Color currentColor = {255, 0, 0, 255}; // Default color: Red

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        currentPoint.x = event.button.x;
                        currentPoint.y = event.button.y;
                        isDrawing = true;
                    }
                    break;
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        isDrawing = false;
                        Line line;
                        line.start = currentPoint;
                        line.end.x = event.button.x;
                        line.end.y = event.button.y;
                        line.color = currentColor;
                        lines.push_back(line);
                    }
                    break;
                 case SDL_EVENT_MOUSE_MOTION:
                    if (isDrawing) {
                        Line line;
                        line.start = currentPoint;
                        line.end.x = event.motion.x;
                        line.end.y = event.motion.y;
                        line.color = currentColor;
                        // "Live" preview -  don't store it yet.  Redraw each time
                        currentPoint.x = event.motion.x;
                        currentPoint.y = event.motion.y;
                    }
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (event.key.keysym.sym == SDLK_r) {
                        currentColor = {255, 0, 0, 255}; // Red
                    } else if (event.key.keysym.sym == SDLK_g) {
                        currentColor = {0, 255, 0, 255}; // Green
                    } else if (event.key.keysym.sym == SDLK_b) {
                        currentColor = {0, 0, 255, 255}; // Blue
                    } else if (event.key.keysym.sym == SDLK_c) {
                        lines.clear(); // Clear the screen
                    }
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Background color: White
        SDL_RenderClear(renderer);

         // Draw stored lines
        for (const auto& line : lines) {
            SDL_SetRenderDrawColor(renderer, line.color.r, line.color.g, line.color.b, line.color.a);
            SDL_RenderLine(renderer, line.start.x, line.start.y, line.end.x, line.end.y);
        }

        // Draw "live" preview line while drawing
        if (isDrawing) {
            SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
            SDL_RenderLine(renderer, currentPoint.x, currentPoint.y, event.motion.x, event.motion.y); // Use latest mouse position
        }


        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
