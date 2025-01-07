#include <iostream>
#include <fstream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Constants
const int TILE_SIZE = 32;
const int SCREEN_WIDTH = 800;  // Window width
const int SCREEN_HEIGHT = 576; // Window height (18 tiles down * TILE_SIZE)

// Player structure
struct Player {
    SDL_Rect rect = { 100, 100, TILE_SIZE, TILE_SIZE }; // Initial position and size
    float velocityY = 0.0f;
    bool onGround = false;
};

// Camera structure
struct Camera {
    int x = 0; // X position of the camera
    int y = 0; // Y position of the camera (optional for vertical scrolling)
    int width = SCREEN_WIDTH;
    int height = SCREEN_HEIGHT;
};

// Function to load the map from a file
std::vector<std::vector<int>> loadMap(const std::string& filename) {
    std::vector<std::vector<int>> map;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::vector<int> row;
        for (char c : line) {
            row.push_back(c - '0'); // Convert char to int
        }
        map.push_back(row);
    }
    return map;
}

// Function to update the camera position
void updateCamera(Camera& camera, const Player& player, int mapWidth, int mapHeight) {
    camera.x = player.rect.x + player.rect.w / 2 - camera.width / 2;
    camera.y = player.rect.y + player.rect.h / 2 - camera.height / 2;

    // Clamp camera position to map boundaries
    if (camera.x < 0) camera.x = 0;
    if (camera.y < 0) camera.y = 0;
    if (camera.x + camera.width > mapWidth * TILE_SIZE) camera.x = mapWidth * TILE_SIZE - camera.width;
    if (camera.y + camera.height > mapHeight * TILE_SIZE) camera.y = mapHeight * TILE_SIZE - camera.height;
}

// Function to render the map
void renderMap(SDL_Renderer* renderer, SDL_Texture* blockTexture, const std::vector<std::vector<int>>& map, const Camera& camera) {
    std::size_t startX = camera.x / TILE_SIZE;
    std::size_t startY = camera.y / TILE_SIZE;
    std::size_t endX = (camera.x + camera.width) / TILE_SIZE + 1;
    std::size_t endY = (camera.y + camera.height) / TILE_SIZE + 1;

    for (std::size_t y = startY; y < endY && y < map.size(); ++y) {
        for (std::size_t x = startX; x < endX && x < map[y].size(); ++x) {
            if (map[y][x] == 1) {
                SDL_Rect dstRect = { 
                    static_cast<int>(x * TILE_SIZE - camera.x), 
                    static_cast<int>(y * TILE_SIZE - camera.y), 
                    TILE_SIZE, TILE_SIZE 
                };
                SDL_RenderCopy(renderer, blockTexture, nullptr, &dstRect);
            }
        }
    }
}

// Function to render the player
void renderPlayer(SDL_Renderer* renderer, SDL_Texture* playerTexture, const Player& player, const Camera& camera) {
    SDL_Rect dstRect = {
        static_cast<int>(player.rect.x - camera.x),
        static_cast<int>(player.rect.y - camera.y),
        player.rect.w,
        player.rect.h
    };
    SDL_RenderCopy(renderer, playerTexture, nullptr, &dstRect);
}

// Function to handle player movement and gravity
void handlePlayerMovement(Player& player, const std::vector<std::vector<int>>& map, float deltaTime) {
    const float GRAVITY = 300.0f; // Gravity strength
    const float JUMP_VELOCITY = -400.0f; // Initial jump velocity
    const float MOVE_SPEED = 400.0f; // Horizontal move speed (pixels per second)

    const Uint8* keys = SDL_GetKeyboardState(nullptr);

    // Horizontal movement
    if (keys[SDL_SCANCODE_LEFT]) {
        player.rect.x -= static_cast<int>(MOVE_SPEED * deltaTime); // Move left
    }
    if (keys[SDL_SCANCODE_RIGHT]) {
        player.rect.x += static_cast<int>(MOVE_SPEED * deltaTime); // Move right
    }

    // Apply gravity
    player.velocityY += GRAVITY * deltaTime; // Gravity affects vertical velocity
    player.rect.y += static_cast<int>(player.velocityY * deltaTime); // Move vertically based on velocity

    // Collision detection with the ground (horizontal and vertical)
    player.onGround = false;
    for (std::size_t y = 0; y < map.size(); ++y) {
        for (std::size_t x = 0; x < map[y].size(); ++x) {
            if (map[y][x] == 1) { // Check if the current tile is solid
                SDL_Rect block = { 
                    static_cast<int>(x * TILE_SIZE), 
                    static_cast<int>(y * TILE_SIZE), 
                    TILE_SIZE, TILE_SIZE 
                };

                // Horizontal collision (left/right movement)
                if (SDL_HasIntersection(&player.rect, &block)) {
                    if (player.rect.x < block.x) {  // Colliding from left side
                        player.rect.x = block.x - player.rect.w;
                    } else if (player.rect.x + player.rect.w > block.x + block.w) {  // Colliding from right side
                        player.rect.x = block.x + block.w;
                    }
                }

                // Vertical collision (up/down movement)
                if (SDL_HasIntersection(&player.rect, &block)) {
                    if (player.velocityY > 0) { // Falling down
                        player.rect.y = block.y - player.rect.h;
                        player.velocityY = 0; // Stop downward velocity
                        player.onGround = true; // The player is standing on the ground
                    }
                }
            }
        }
    }

    // Jumping logic
    if (player.onGround && keys[SDL_SCANCODE_SPACE]) {
        player.velocityY = JUMP_VELOCITY; // Apply jump velocity when the player is on the ground
    }
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Error initializing SDL: " << SDL_GetError() << std::endl;
        return 1;
    }
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        std::cerr << "Error initializing SDL_image: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!window || !renderer) {
        std::cerr << "Error creating SDL window or renderer: " << SDL_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Texture* blockTexture = IMG_LoadTexture(renderer, "block.png");
    SDL_Texture* playerTexture = IMG_LoadTexture(renderer, "player.png");

    if (!blockTexture || !playerTexture) {
        std::cerr << "Error loading textures: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    std::vector<std::vector<int>> map = loadMap("map.txt");
    Player player;
    Camera camera;

    Uint32 lastTime = SDL_GetTicks();
    bool running = true;

    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // Cap deltaTime to prevent erratic movement
        const float MAX_DELTA_TIME = 0.05f;  // 50ms per frame
        deltaTime = std::min(deltaTime, MAX_DELTA_TIME);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        handlePlayerMovement(player, map, deltaTime);
        updateCamera(camera, player, map[0].size(), map.size());

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render map and player
        renderMap(renderer, blockTexture, map, camera);
        renderPlayer(renderer, playerTexture, player, camera);

        // Present the rendered frame
        SDL_RenderPresent(renderer);
	SDL_Delay(15);
    }

    // Cleanup
    SDL_DestroyTexture(blockTexture);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

