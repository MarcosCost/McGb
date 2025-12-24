#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h> // Essential for SDL3
#include <iostream>

const int WIDTH = 160 * 3;
const int HEIGHT = 144 * 3;

int main(int argc, char* argv[]) {
    // 1. Start SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL failed to start: " << SDL_GetError() << std::endl;
        return 1;
    }

    // 2. Create Window
    SDL_Window* window = SDL_CreateWindow("McGB Window Test", WIDTH, HEIGHT, 0);
    if (!window) {
        std::cout << "Window failed to open: " << SDL_GetError() << std::endl;
        return 1;
    }

    // 3. Create Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cout << "Renderer failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    bool running = true;
    SDL_Event event;

    // 4. The Loop
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    // 5. Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}