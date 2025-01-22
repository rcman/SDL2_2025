#include <SDL2/SDL.h>
#include <vector>

// Structure for platforms
struct Platform {
    SDL_Rect rect;
    bool isMoving;
    float moveSpeed;
    SDL_FPoint initialPosition;
    SDL_FPoint moveDirection; // Normalized direction vector
};

// Structure for enemies
struct Enemy {
    SDL_Rect rect;
    float moveSpeed;
    SDL_FPoint initialPosition;
    SDL_FPoint moveDirection; // Normalized direction vector
};

// Structure for collectibles
struct Collectible {
    SDL_Rect rect; // Using rect for simplicity, could use circle
    bool isCollected;
};

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 3;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window: %s", SDL_GetError());
        return 3;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create renderer: %s", SDL_GetError());
        return 3;
    }

    // Player setup
    SDL_Rect playerRect = { 100, 500, 50, 50 }; 
    float playerSpeed = 5.0f; // Adjusted for SDL
    float jumpForce = -15.0f;
    float playerVelocityY = 0.0f;
    const float gravity = 0.5f;
    bool isJumping = false;

    // Ground setup
    SDL_Rect groundRect = { 0, 550, 800, 50 };

    // Platforms
    std::vector<Platform> platforms;

    // Static platforms
    Platform platform1 = { { 300, 400, 100, 20 }, false, 0, { 300, 400 }, { 0, 0 } };
    platforms.push_back(platform1);

    Platform platform2 = { { 500, 300, 150, 20 }, false, 0, { 500, 300 }, { 0, 0 } };
    platforms.push_back(platform2);

    // Moving platform
    Platform movingPlatform = { { 200, 200, 100, 20 }, true, 2.0f, { 200, 200 }, { 1, 0 } };
    platforms.push_back(movingPlatform);

    // Enemies
    std::vector<Enemy> enemies;

    Enemy enemy1 = { { 400, 510, 40, 40 }, 3.0f, { 400, 510 }, { 1, 0 } };
    enemies.push_back(enemy1);

    // Collectibles
    std::vector<Collectible> collectibles;

    Collectible collectible1 = { { 600, 250, 30, 30 }, false }; // Using rect for simplicity
    collectibles.push_back(collectible1);

    // Score
    int score = 0;
    // (SDL_ttf would be needed for text rendering, omitted for simplicity)

    // Game loop
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Player movement
        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
        if (currentKeyStates[SDL_SCANCODE_LEFT]) {
            playerRect.x -= playerSpeed;
        }
        if (currentKeyStates[SDL_SCANCODE_RIGHT]) {
            playerRect.x += playerSpeed;
        }
        if (currentKeyStates[SDL_SCANCODE_SPACE] && !isJumping) {
            playerVelocityY = jumpForce;
            isJumping = true;
        }

        // Apply gravity
        playerVelocityY += gravity;
        playerRect.y += playerVelocityY;

        // Collision with ground
        if (SDL_HasIntersection(&playerRect, &groundRect)) {
            playerRect.y = groundRect.y - playerRect.h;
            playerVelocityY = 0;
            isJumping = false;
        }

        // Collision with platforms
        for (auto& platform : platforms) {
            if (SDL_HasIntersection(&playerRect, &platform.rect)) {
                if (playerVelocityY > 0 && playerRect.y < platform.rect.y) {
                    playerRect.y = platform.rect.y - playerRect.h;
                    playerVelocityY = 0;
                    isJumping = false;
                }
            }
        }

        // Move moving platforms
        for (auto& platform : platforms) {
            if (platform.isMoving) {
                platform.rect.x += platform.moveSpeed * platform.moveDirection.x;
                platform.rect.y += platform.moveSpeed * platform.moveDirection.y;

                if (platform.rect.x < 0 || platform.rect.x + platform.rect.w > 800) {
                    platform.moveDirection.x *= -1;
                }
                if (platform.rect.y < 0 || platform.rect.y + platform.rect.h > 600) {
                    platform.moveDirection.y *= -1;
                }
            }
        }

        // Move enemies
        for (auto& enemy : enemies) {
            enemy.rect.x += enemy.moveSpeed * enemy.moveDirection.x;

            if (enemy.rect.x < 0 || enemy.rect.x + enemy.rect.w > 800) {
                enemy.moveDirection.x *= -1;
            }
        }

        // Collision with enemies
        for (auto& enemy : enemies) {
            if (SDL_HasIntersection(&playerRect, &enemy.rect)) {
                playerRect.x = 100;
                playerRect.y = 500;
                playerVelocityY = 0;
            }
        }

        // Collect collectibles
        for (auto& collectible : collectibles) {
            if (!collectible.isCollected && SDL_HasIntersection(&playerRect, &collectible.rect)) {
                collectible.isCollected = true;
                score += 10;
            }
        }

        // Keep player within screen bounds
        if (playerRect.x < 0) {
            playerRect.x = 0;
        }
        if (playerRect.x + playerRect.w > 800) {
            playerRect.x = 800 - playerRect.w;
        }

        // Rendering
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear screen (black)
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Brown
        SDL_RenderFillRect(renderer, &groundRect);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
        for (const auto& platform : platforms) {
            if (!platform.isMoving) {
                SDL_RenderFillRect(renderer, &platform.rect);
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue
        for (const auto& platform : platforms) {
            if (platform.isMoving) {
                SDL_RenderFillRect(renderer, &platform.rect);
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255); // Magenta
        for (const auto& enemy : enemies) {
            SDL_RenderFillRect(renderer, &enemy.rect);
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow
        for (const auto& collectible : collectibles) {
            if (!collectible.isCollected) {
                SDL_RenderFillRect(renderer, &collectible.rect); // Using rect
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green
        SDL_RenderFillRect(renderer, &playerRect);

        // (SDL_ttf would be needed to render scoreText)

        SDL_RenderPresent(renderer);
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

