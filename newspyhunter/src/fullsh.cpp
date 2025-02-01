#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <vector>

// Screen dimensions
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Player car settings
const int CAR_WIDTH = 60;
const int CAR_HEIGHT = 30;
const int CAR_SPEED = 5;

// Enemy car settings
const int ENEMY_CAR_WIDTH = 50;
const int ENEMY_CAR_HEIGHT = 30;
const int ENEMY_CAR_SPEED = 3;

// 18-wheeler settings
const int TRUCK_WIDTH = 100;
const int TRUCK_HEIGHT = 60;
const int TRUCK_SPEED = 2;

// Bullet settings
const int BULLET_SPEED = 10;

// Function declarations
bool init();
bool loadMedia();
void close();
bool checkCollision(SDL_Rect a, SDL_Rect b);

// SDL objects
SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;

// Texture wrapper class
class LTexture {
public:
    LTexture();
    ~LTexture();

    bool loadFromFile(std::string path);
    void free();
    void render(int x, int y, SDL_Rect* clip = nullptr, double angle = 0.0, SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);
    int getWidth();
    int getHeight();

private:
    SDL_Texture* mTexture;
    int mWidth;
    int mHeight;
};

// The player-controlled car
class Car {
public:
    Car();

    void handleEvent(SDL_Event& e);
    void move();
    void render();
    SDL_Rect getCollider();
    void setPosition(int x, int y);

private:
    int mPosX, mPosY;
    int mVelX;
    SDL_Rect mCollider;
};

// The bullet that the car fires
class Bullet {
public:
    static const int BULLET_WIDTH = 5;
    static const int BULLET_HEIGHT = 10;

    Bullet(int x, int y);

    void move();
    void render();
    SDL_Rect getCollider();
    bool isActive() const;
    void setActive(bool active);

private:
    int mPosX, mPosY;
    int mVelX, mVelY;
    SDL_Rect mCollider;
    bool active;
};

// The enemy car
class EnemyCar {
public:
    EnemyCar(int lane);

    void move();
    void render();
    SDL_Rect getCollider();
    bool isAlive() const;
    void setAlive(bool alive);

private:
    int mPosX, mPosY;
    int mVelY;
    SDL_Rect mCollider;
    bool alive;
};

// The 18-wheeler truck
class Truck {
public:
    Truck();

    void move();
    void render();
    SDL_Rect getCollider();
    bool isOnScreen() const;
    void setOnScreen(bool onScreen);

private:
    int mPosX, mPosY;
    int mVelY;
    SDL_Rect mCollider;
    bool onScreen;
};

// Globally used textures
LTexture gCarTexture;
LTexture gEnemyCarTexture;
LTexture gTruckTexture;
LTexture gBulletTexture;
LTexture gBackgroundTexture;

// Sound effects
Mix_Chunk* gEngineSound = nullptr;
Mix_Chunk* gShootSound = nullptr;
Mix_Chunk* gExplosionSound = nullptr;

// Texture methods
LTexture::LTexture() : mTexture(nullptr), mWidth(0), mHeight(0) {}

LTexture::~LTexture() {
    free();
}

bool LTexture::loadFromFile(std::string path) {
    free();
    SDL_Texture* newTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (!loadedSurface) {
        std::cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
    } else {
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (!newTexture) {
            std::cerr << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << std::endl;
        } else {
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }
        SDL_FreeSurface(loadedSurface);
    }
    mTexture = newTexture;
    return mTexture != nullptr;
}

void LTexture::free() {
    if (mTexture) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
    SDL_Rect renderQuad = { x, y, mWidth, mHeight };
    if (clip) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth() {
    return mWidth;
}

int LTexture::getHeight() {
    return mHeight;
}

// Car methods
Car::Car() : mPosX(SCREEN_WIDTH / 2 - CAR_WIDTH / 2), mPosY(SCREEN_HEIGHT - CAR_HEIGHT - 10), mVelX(0) {
    mCollider.w = CAR_WIDTH;
    mCollider.h = CAR_HEIGHT;
}

void Car::handleEvent(SDL_Event& e) {
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
        case SDLK_LEFT: mVelX = -CAR_SPEED; break;
        case SDLK_RIGHT: mVelX = CAR_SPEED; break;
        }
    } else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
        if (e.key.keysym.sym == SDLK_LEFT || e.key.keysym.sym == SDLK_RIGHT) {
            mVelX = 0;
        }
    }
}

void Car::move() {
    mPosX += mVelX;
    mCollider.x = mPosX;
    if (mPosX < 0 || mPosX + CAR_WIDTH > SCREEN_WIDTH) {
        mPosX -= mVelX;
        mCollider.x = mPosX;
    }
}

void Car::render() {
    gCarTexture.render(mPosX, mPosY);
}

SDL_Rect Car::getCollider() {
    return mCollider;
}

void Car::setPosition(int x, int y) {
    mPosX = x;
    mPosY = y;
    mCollider.x = mPosX;
    mCollider.y = mPosY;
}

// Bullet methods
Bullet::Bullet(int x, int y) : mPosX(x), mPosY(y), mVelX(0), mVelY(-BULLET_SPEED), active(true) {
    mCollider.w = BULLET_WIDTH;
    mCollider.h = BULLET_HEIGHT;
}

void Bullet::move() {
    mPosY += mVelY;
    mCollider.y = mPosY;
    if (mPosY < 0) {
        active = false;
    }
}

void Bullet::render() {
    gBulletTexture.render(mPosX, mPosY);
}

SDL_Rect Bullet::getCollider() {
    return mCollider;
}

bool Bullet::isActive() const {
    return active;
}

void Bullet::setActive(bool active) {
    this->active = active;
}

// EnemyCar methods
EnemyCar::EnemyCar(int lane) : mVelY(ENEMY_CAR_SPEED), alive(true) {
    mPosX = lane * (SCREEN_WIDTH / 3) + (SCREEN_WIDTH / 6) - ENEMY_CAR_WIDTH / 2;
    mPosY = -ENEMY_CAR_HEIGHT;
    mCollider.w = ENEMY_CAR_WIDTH;
    mCollider.h = ENEMY_CAR_HEIGHT;
}

void EnemyCar::move() {
    mPosY += mVelY;
    mCollider.y = mPosY;
    if (mPosY > SCREEN_HEIGHT) {
        alive = false;
    }
}

void EnemyCar::render() {
    gEnemyCarTexture.render(mPosX, mPosY);
}

SDL_Rect EnemyCar::getCollider() {
    return mCollider;
}

bool EnemyCar::isAlive() const {
    return alive;
}

void EnemyCar::setAlive(bool alive) {
    this->alive = alive;
}

// Truck methods
Truck::Truck() : mPosX(SCREEN_WIDTH / 2 - TRUCK_WIDTH / 2), mPosY(-TRUCK_HEIGHT), mVelY(TRUCK_SPEED), onScreen(false) {
    mCollider.w = TRUCK_WIDTH;
    mCollider.h = TRUCK_HEIGHT;
}

void Truck::move() {
    mPosY += mVelY;
    mCollider.y = mPosY;
    if (mPosY > SCREEN_HEIGHT) {
        onScreen = false;
    }
}

void Truck::render() {
    gTruckTexture.render(mPosX, mPosY);
}

SDL_Rect Truck::getCollider() {
    return mCollider;
}

bool Truck::isOnScreen() const {
    return onScreen;
}

void Truck::setOnScreen(bool onScreen) {
    this->onScreen = onScreen;
}

// SDL initialization functions
bool init() {
    bool success = true;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        success = false;
    } else {
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            std::cerr << "Warning: Linear texture filtering not enabled!" << std::endl;
        }
        gWindow = SDL_CreateWindow("Spy Hunter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (!gWindow) {
            std::cerr << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
            success = false;
        } else {
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (!gRenderer) {
                std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
                success = false;
            } else {
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags)) {
                    std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
                    success = false;
                }
                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
                    std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
                    success = false;
                }
            }
        }
    }
    return success;
}

// Media loading functions
bool loadMedia() {
    bool success = true;
    if (!gCarTexture.loadFromFile("car.bmp")) {
        std::cerr << "Failed to load car texture!" << std::endl;
        success = false;
    }
    if (!gEnemyCarTexture.loadFromFile("enemy_car.bmp")) {
        std::cerr << "Failed to load enemy car texture!" << std::endl;
        success = false;
    }
    if (!gTruckTexture.loadFromFile("truck.bmp")) {
        std::cerr << "Failed to load truck texture!" << std::endl;
        success = false;
    }
    if (!gBulletTexture.loadFromFile("bullet.bmp")) {
        std::cerr << "Failed to load bullet texture!" << std::endl;
        success = false;
    }
    if (!gBackgroundTexture.loadFromFile("background.bmp")) {
        std::cerr << "Failed to load background texture!" << std::endl;
        success = false;
    }
    gEngineSound = Mix_LoadWAV("engine.wav");
    if (!gEngineSound) {
        std::cerr << "Failed to load engine sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        success = false;
    }
    gShootSound = Mix_LoadWAV("shoot.wav");
    if (!gShootSound) {
        std::cerr << "Failed to load shoot sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        success = false;
    }
    gExplosionSound = Mix_LoadWAV("explosion.wav");
    if (!gExplosionSound) {
        std::cerr << "Failed to load explosion sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        success = false;
    }
    return success;
}

// Clean up
void close() {
    Mix_FreeChunk(gEngineSound);
    Mix_FreeChunk(gShootSound);
    Mix_FreeChunk(gExplosionSound);
    gEngineSound = nullptr;
    gShootSound = nullptr;
    gExplosionSound = nullptr;
    gCarTexture.free();
    gEnemyCarTexture.free();
    gTruckTexture.free();
    gBulletTexture.free();
    gBackgroundTexture.free();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

// Collision detection
bool checkCollision(SDL_Rect a, SDL_Rect b) {
    int leftA = a.x;
    int rightA = a.x + a.w;
    int topA = a.y;
    int bottomA = a.y + a.h;
    int leftB = b.x;
    int rightB = b.x + b.w;
    int topB = b.y;
    int bottomB = b.y + b.h;

    return !(leftA > rightB || rightA < leftB || topA > bottomB || bottomA < topB);
}

// Main function
int main(int argc, char* args[]) {
    if (!init()) {
        std::cerr << "Failed to initialize!" << std::endl;
    } else {
        if (!loadMedia()) {
            std::cerr << "Failed to load media!" << std::endl;
        } else {
            bool quit = false;
            SDL_Event e;
            Car playerCar;
            std::vector<Bullet> bullets;
            std::vector<EnemyCar> enemyCars;
            Truck truck;
            int score = 0;

            while (!quit) {
                while (SDL_PollEvent(&e) != 0) {
                    if (e.type == SDL_QUIT) {
                        quit = true;
                    }
                    playerCar.handleEvent(e);
                }

                playerCar.move();

                // Add new enemy cars
                if (rand() % 100 < 2) {
                    int lane = rand() % 3;
                    enemyCars.push_back(EnemyCar(lane));
                }

                // Move enemy cars
                for (auto& car : enemyCars) {
                    car.move();
                    if (checkCollision(playerCar.getCollider(), car.getCollider())) {
                        Mix_PlayChannel(-1, gExplosionSound, 0);
                        quit = true;
                    }
                }

                // Move bullets
                for (auto it = bullets.begin(); it != bullets.end();) {
                    it->move();
                    if (!it->isActive()) {
                        it = bullets.erase(it);
                    } else {
                        ++it;
                    }
                }

                // Move and render truck
                if (!truck.isOnScreen()) {
                    truck = Truck();
                    truck.setOnScreen(true);
                }
                truck.move();

                // Clear screen
                SDL_RenderClear(gRenderer);

                // Render background
                gBackgroundTexture.render(0, 0);

                // Render player car
                playerCar.render();

                // Render enemy cars
                for (auto& car : enemyCars) {
                    car.render();
                }

                // Render truck
                truck.render();

                // Render bullets
                for (auto& bullet : bullets) {
                    bullet.render();
                }

                // Update screen
                SDL_RenderPresent(gRenderer);
            }
        }
    }

    close();
    return 0;
}
