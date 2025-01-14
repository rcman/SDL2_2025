#include <SDL2/SDL.h>
#include <vector>
#include <iostream>

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Define speeds
const int PLAYER_SPEED = 5;
const int BULLET_SPEED = 10;
const int ENEMY_SPEED = 2;

// Helper function for collision detection
bool checkCollision(SDL_Rect a, SDL_Rect b) {
    return (a.x < b.x + b.w &&
            a.x + a.w > b.x &&
            a.y < b.y + b.h &&
            a.y + a.h > b.y);
}

// Class for the player
class Player {
public:
    SDL_Rect rect; // Player position and size
    SDL_Texture* texture;

    Player(SDL_Renderer* renderer) {
        rect = {100, SCREEN_HEIGHT / 2 - 25, 50, 50};
        SDL_Surface* surface = SDL_LoadBMP("player.bmp");
        if (!surface) {
            std::cerr << "Failed to load player asset: " << SDL_GetError() << std::endl;
        }
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    void handleInput(const Uint8* keyState) {
        if (keyState[SDL_SCANCODE_UP] && rect.y > 0) rect.y -= PLAYER_SPEED;
        if (keyState[SDL_SCANCODE_DOWN] && rect.y < SCREEN_HEIGHT - rect.h) rect.y += PLAYER_SPEED;
        if (keyState[SDL_SCANCODE_LEFT] && rect.x > 0) rect.x -= PLAYER_SPEED;
        if (keyState[SDL_SCANCODE_RIGHT] && rect.x < SCREEN_WIDTH - rect.w) rect.x += PLAYER_SPEED;
    }

    void render(SDL_Renderer* renderer) {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }

    ~Player() {
        SDL_DestroyTexture(texture);
    }
};

// Class for projectiles
class Bullet {
public:
    SDL_Rect rect;
    SDL_Texture* texture;
    bool active;

    Bullet(SDL_Renderer* renderer, int x, int y) {
        rect = {x, y, 10, 10};
        SDL_Surface* surface = SDL_LoadBMP("bullet.bmp");
        if (!surface) {
            std::cerr << "Failed to load bullet asset: " << SDL_GetError() << std::endl;
        }
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        active = true;
    }

    void update() {
        rect.x += BULLET_SPEED;
        if (rect.x > SCREEN_WIDTH) active = false;
    }

    void render(SDL_Renderer* renderer) {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }

    ~Bullet() {
        SDL_DestroyTexture(texture);
    }
};

// Class for enemies
class Enemy {
public:
    SDL_Rect rect;
    SDL_Texture* texture;
    bool active;

    Enemy(SDL_Renderer* renderer, int x, int y) {
        rect = {x, y, 50, 50};
        SDL_Surface* surface = SDL_LoadBMP("enemy.bmp");
        if (!surface) {
            std::cerr << "Failed to load enemy asset: " << SDL_GetError() << std::endl;
        }
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        active = true;
    }

    void update() {
        rect.x -= ENEMY_SPEED;
        if (rect.x + rect.w < 0) active = false;
    }

    void render(SDL_Renderer* renderer) {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }

    ~Enemy() {
        SDL_DestroyTexture(texture);
    }
};

// Main game function
int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Harrier Clone", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize game objects
    Player player(renderer);
    std::vector<Bullet*> bullets;
    std::vector<Enemy*> enemies;

    bool running = true;
    SDL_Event event;
    int enemySpawnTimer = 0;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            // Handle shooting
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                bullets.push_back(new Bullet(renderer, player.rect.x + player.rect.w, player.rect.y + player.rect.h / 2 - 5));
            }
        }

        const Uint8* keyState = SDL_GetKeyboardState(NULL);
        player.handleInput(keyState);

        // Update bullets
        for (auto it = bullets.begin(); it != bullets.end();) {
            (*it)->update();
            if (!(*it)->active) {
                delete *it;
                it = bullets.erase(it);
            } else {
                ++it;
            }
        }

        // Spawn enemies
        enemySpawnTimer++;
        if (enemySpawnTimer > 20) { // Adjust spawn rate here
            enemies.push_back(new Enemy(renderer, SCREEN_WIDTH, rand() % (SCREEN_HEIGHT - 50)));
            enemySpawnTimer = 0;
        }

        // Update enemies
        for (auto it = enemies.begin(); it != enemies.end();) {
            (*it)->update();
            if (!(*it)->active) {
                delete *it;
                it = enemies.erase(it);
            } else {
                ++it;
            }
        }

        // Check collisions
        for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
            bool bulletRemoved = false;
            for (auto enemyIt = enemies.begin(); enemyIt != enemies.end();) {
                if (checkCollision((*bulletIt)->rect, (*enemyIt)->rect)) {
                    delete *bulletIt;
                    bulletIt = bullets.erase(bulletIt);
                    delete *enemyIt;
                    enemyIt = enemies.erase(enemyIt);
                    bulletRemoved = true;
                    break;
                } else {
                    ++enemyIt;
                }
            }
            if (!bulletRemoved) ++bulletIt;
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        player.render(renderer);
        for (auto& bullet : bullets) bullet->render(renderer);
        for (auto& enemy : enemies) enemy->render(renderer);

        SDL_RenderPresent(renderer);
    }

    // Clean up
    for (auto& bullet : bullets) delete bullet;
    for (auto& enemy : enemies) delete enemy;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

