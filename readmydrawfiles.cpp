#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// Assuming your data structures look something like this (adjust as needed)
struct Shape {
    // ... your 272 bytes of shape data ...
    unsigned char data[272]; // Example: Using a fixed-size array
    int flag;
};

struct AnimObject {
    int max;
    std::vector<Shape*> fshp; // Use a vector to dynamically store shapes
};

AnimObject animobjects[10]; // Array to hold animation objects

// Function to clear the animobjects data
void ClearData() {
    for (int i = 0; i < 10; ++i) {
        animobjects[i].max = 0;
        for (Shape* shape : animobjects[i].fshp) {
            delete shape; // Free the dynamically allocated shapes
        }
        animobjects[i].fshp.clear(); // Clear the vector
    }
}


int fileread() {
    int numobj, numrow;
    std::string label;
    std::string infst;

    ClearData();

    std::cout << "Please enter Filename: ";
    std::cin >> label;

    infst = label;
    label += ".dat";
    infst += ".inf";

    std::ifstream in_inf(infst, std::ios::binary); // Open in binary mode for .inf
    std::ifstream in_data(label, std::ios::binary); // Open in binary mode for .dat

    if (!in_inf.is_open()) {
        std::cerr << "Cannot open info file: " << infst << std::endl;
        return 1;
    }

    if (!in_data.is_open()) {
        std::cerr << "Cannot open data file: " << label << std::endl;
        in_inf.close(); // Important: Close the other file if it's open
        return 1;
    }


    for (numrow = 0; numrow <= 9; numrow++) {
        in_inf >> animobjects[numrow].max;  // Read max from .inf
        in_inf.ignore(1); // Consume the newline character after reading max

        animobjects[numrow].fshp.resize(animobjects[numrow].max + 1); // Resize the vector

        for (numobj = 0; numobj <= animobjects[numrow].max; numobj++) {
            animobjects[numrow].fshp[numobj] = new Shape; // Allocate memory for each shape
            in_data.read(reinterpret_cast<char*>(animobjects[numrow].fshp[numobj]->data), 272);
            animobjects[numrow].fshp[numobj]->flag = 1;
        }
    }

    in_inf.close();
    in_data.close();

    return 1;
}


int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // ... (Rest of your SDL initialization and drawing code) ...

    fileread(); // Call the file reading function

    // ... (Use the data read from the files in your SDL drawing loop) ...

    SDL_Quit();
    return 0;
}
