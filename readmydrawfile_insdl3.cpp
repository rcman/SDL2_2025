#include <SDL3/SDL.h>
#include <SDL3_ttf.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem> // For file system operations (C++17 and later)
#ifdef _WIN32 // Include for windows systems
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

// Shape structure (adjust as needed)
struct Shape {
    unsigned char data[272];
    int flag;
};

// AnimObject structure (adjust as needed)
struct AnimObject {
    int max;
    std::vector<Shape*> fshp;
};

AnimObject animobjects[10];

// Clear data function
void ClearData() {
    for (int i = 0; i < 10; ++i) {
        animobjects[i].max = 0;
        for (Shape* shape : animobjects[i].fshp) {
            delete shape;
        }
        animobjects[i].fshp.clear();
    }
}

// File reading function
int fileread(const std::string& filename) {
    ClearData();

    std::ifstream in_inf(filename + ".inf", std::ios::binary);
    std::ifstream in_data(filename + ".std", std::ios::binary);

    if (!in_inf.is_open()) {
        std::cerr << "Cannot open info file: " << filename + ".inf" << std::endl;
        return 1;
    }

    if (!in_data.is_open()) {
        std::cerr << "Cannot open data file: " << filename + ".std" << std::endl;
        in_inf.close();
        return 1;
    }

    for (int numrow = 0; numrow <= 9; numrow++) {
        in_inf >> animobjects[numrow].max;
        in_inf.ignore(1); // Consume newline

        animobjects[numrow].fshp.resize(animobjects[numrow].max + 1);

        for (int numobj = 0; numobj <= animobjects[numrow].max; numobj++) {
            animobjects[numrow].fshp[numobj] = new Shape;
            in_data.read(reinterpret_cast<char*>(animobjects[numrow].fshp[numobj]->data), 272);
            animobjects[numrow].fshp[numobj]->flag = 1;
        }
    }

    in_inf.close();
    in_data.close();

    return 0;
}

// Show message box function
void showMessageBox(const std::string& title, const std::string& message) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), message.c_str(), nullptr);
}

// List files in directory
std::vector<std::string> listFilesInDirectory(const std::string& directory, const std::string& extension) {
    std::vector<std::string> files;

    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == extension) {
            files.push_back(entry.path().stem().string()); // Store filename without extension
        }
    }
    return files;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() < 0) {
        std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("SDL3 File Loader", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr, SDL_RENDERER_ACCELERATED);

    TTF_Font* font = TTF_OpenFont("arial.ttf", 24); // Replace with your font file
    if (!font) {
        std::cerr << "Font loading failed: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    std::vector<std::string> fileList;
    int selectedFile = -1;

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_f) {
                        char buff[FILENAME_MAX];
                        GetCurrentDir(buff, FILENAME_MAX);
                        std::string current_dir(buff);
                        fileList = listFilesInDirectory(current_dir, ".std");
                        selectedFile = -1;
                    } else if (event.key.keysym.sym == SDLK_RETURN && selectedFile != -1) {
                        if (fileread(fileList[selectedFile]) == 0) {
                            std::cout << "File loaded successfully: " << fileList[selectedFile] << std::endl;
                        } else {
                            showMessageBox("Error", "Failed to load file.");
                        }
                    } else if (event.key.keysym.sym == SDLK_UP) {
                        selectedFile = (selectedFile - 1 + fileList.size()) % fileList.size();
                    } else if (event.key.keysym.sym == SDLK_DOWN) {
                        selectedFile = (selectedFile + 1) % fileList.size();
                    }
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        int yOffset = 50;
        for (size_t i = 0; i < fileList.size(); ++i) {
            SDL_Color color = (i == selectedFile) ? SDL_Color{255, 255, 0, 255} : SDL_Color{255, 255, 255, 255};
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, fileList[i].c_str(), color);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = {50, yOffset, textSurface->w, textSurface->h};
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
            SDL_DestroyTexture(textTexture);
            SDL_FreeSurface(textSurface);
            yOffset += 30;
        }

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
