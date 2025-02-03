#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <fstream>
#include <vector>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int BLOCK_SIZE = 32;
const float GRAVITY = 0.5f;
const float JUMP_STRENGTH = -10.0f;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

struct Player {
    float x, y;
    float velX, velY;
    bool onGround;
    int width, height;

    Player() : x(100), y(100), velX(0), velY(0), onGround(false), width(32), height(32) {}

    void update(std::vector<std::vector<int>>& map) {
        // Apply gravity
        if (!onGround) {
            velY += GRAVITY;
        }

        // Update position
        x += velX;
        y += velY;

        // Collision detection
        onGround = false;
        int leftTile = x / BLOCK_SIZE;
        int rightTile = (x + width - 1) / BLOCK_SIZE;
        int topTile = y / BLOCK_SIZE;
        int bottomTile = (y + height - 1) / BLOCK_SIZE;

        for (int i = topTile; i <= bottomTile; ++i) {
            for (int j = leftTile; j <= rightTile; ++j) {
                if (i >= 0 && i < map.size() && j >= 0 && j < map[i].size() && map[i][j] == 1) {
                    if (velY > 0 && y + height > i * BLOCK_SIZE) {
                        y = i * BLOCK_SIZE - height;
                        velY = 0;
                        onGround = true;
                    } else if (velY < 0 && y < (i + 1) * BLOCK_SIZE) {
                        y = (i + 1) * BLOCK_SIZE;
                        velY = 0;
                    }
                }
            }
        }

        // Horizontal collision
        for (int i = topTile; i <= bottomTile; ++i) {
            for (int j = leftTile; j <= rightTile; ++j) {
                if (i >= 0 && i < map.size() && j >= 0 && j < map[i].size() && map[i][j] == 1) {
                    if (velX > 0 && x + width > j * BLOCK_SIZE) {
                        x = j * BLOCK_SIZE - width;
                        velX = 0;
                    } else if (velX < 0 && x < (j + 1) * BLOCK_SIZE) {
                        x = (j + 1) * BLOCK_SIZE;
                        velX = 0;
                    }
                }
            }
        }
    }

    void render() {
        SDL_Rect rect = { static_cast<int>(x), static_cast<int>(y), width, height };
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
};

std::vector<std::vector<int>> loadMap(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::vector<int>> map;
    std::string line;

    while (std::getline(file, line)) {
        std::vector<int> row;
        for (char ch : line) {
            if (ch == '0' || ch == '1') {
                row.push_back(ch - '0');
            }
        }
        map.push_back(row);
    }

    return map;
}

void renderMap(const std::vector<std::vector<int>>& map) {
    for (int i = 0; i < map.size(); ++i) {
        for (int j = 0; j < map[i].size(); ++j) {
            if (map[i][j] == 1) {
                SDL_Rect rect = { j * BLOCK_SIZE, i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    window = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    std::vector<std::vector<int>> map = loadMap("map.txt");
    Player player;

    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_SPACE:
                        if (player.onGround) {
                            player.velY = JUMP_STRENGTH;
                            player.onGround = false;
                        }
                        break;
                    case SDLK_LEFT:
                        player.velX = -3;
                        break;
                    case SDLK_RIGHT:
                        player.velX = 3;
                        break;
                }
            } else if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                    case SDLK_RIGHT:
                        player.velX = 0;
                        break;
                }
            }
        }

        player.update(map);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        renderMap(map);
        player.render();

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
/*
### Adding More Block Types:
If you want to add more block types (e.g., blocks with different behaviors), you can extend the `map.txt` format and modify the collision logic. For example:
- Use `2` for slippery blocks.
- Use `3` for deadly blocks.

Then, update the `Player::update` function to handle these new block types.
*/
with #include <SDL.h>
#include <iostream>
#include <fstream>
#include <vector>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int BLOCK_SIZE = 32;
const float GRAVITY = 0.5f;
const float JUMP_STRENGTH = -10.0f;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

struct Player {
    float x, y;
    float velX, velY;
    bool onGround;
    int width, height;

    Player() : x(100), y(100), velX(0), velY(0), onGround(false), width(32), height(32) {}

    void update(std::vector<std::vector<int>>& map);
    void render();
};

void Player::update(std::vector<std::vector<int>>& map) {
    // Apply gravity
    if (!onGround) {
        velY += GRAVITY;
    }

    // Update position
    x += velX;
    y += velY;

    // Collision detection
    onGround = false;
    int leftTile = x / BLOCK_SIZE;
    int rightTile = (x + width - 1) / BLOCK_SIZE;
    int topTile = y / BLOCK_SIZE;
    int bottomTile = (y + height - 1) / BLOCK_SIZE;

    for (int i = topTile; i <= bottomTile; ++i) {
        for (int j = leftTile; j <= rightTile; ++j) {
            if (i >= 0 && i < map.size() && j >= 0 && j < map[i].size() && map[i][j] == 1) {
                if (velY > 0 && y + height > i * BLOCK_SIZE) {
                    y = i * BLOCK_SIZE - height;
                    velY = 0;
                    onGround = true;
                } else if (velY < 0 && y < (i + 1) * BLOCK_SIZE) {
                    y = (i + 1) * BLOCK_SIZE;
                    velY = 0;
                }
            }
        }
    }

    // Horizontal collision
    for (int i = topTile; i <= bottomTile; ++i) {
        for (int j = leftTile; j <= rightTile; ++j) {
            if (i >= 0 && i < map.size() && j >= 0 && j < map[i].size() && map[i][j] == 1) {
                if (velX > 0 && x + width > j * BLOCK_SIZE) {
                    x = j * BLOCK_SIZE - width;
                    velX = 0;
                } else if (velX < 0 && x < (j + 1) * BLOCK_SIZE) {
                    x = (j + 1) * BLOCK_SIZE;
                    velX = 0;
                }
            }
        }
    }
}

void Player::render() {
    SDL_Rect rect = { static_cast<int>(x), static_cast<int>(y), width, height };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
}

std::vector<std::vector<int>> loadMap(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::vector<int>> map;
    std::string line;

    while (std::getline(file, line)) {
        std::vector<int> row;
        for (char ch : line) {
            if (ch == '0' || ch == '1') {
                row.push_back(ch - '0');
            }
        }
        map.push_back(row);
    }

    return map;
}

void renderMap(const std::vector<std::vector<int>>& map) {
    for (int i = 0; i < map.size(); ++i) {
        for (int j = 0; j < map[i].size(); ++j) {
            if (map[i][j] == 1) {
                SDL_Rect rect = { j * BLOCK_SIZE, i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    window = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    std::vector<std::vector<int>> map = loadMap("map.txt");
    Player player;

    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_SPACE:
                        if (player.onGround) {
                            player.velY = JUMP_STRENGTH;
                            player.onGround = false;
                        }
                        break;
                    case SDLK_LEFT:
                        player.velX = -3;
                        break;
                    case SDLK_RIGHT:
                        player.velX = 3;
                        break;
                }
            } else if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                    case SDLK_RIGHT:
                        player.velX = 0;
                        break;
                }
            }
        }

        player.update(map);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        renderMap(map);
        player.render();

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}




