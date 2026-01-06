#include <iostream>
#include <filesystem>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h> // Essential for SDL3
#include <spdlog/sinks/rotating_file_sink.h>

#include "CPU.h"



const int WIDTH = 160 * 3;
const int HEIGHT = 144 * 3;

void setup_logger() {
    try {
        // Create logs directory if it doesn't exist
        std::filesystem::create_directories("logs");
        
        // Rotating file sink: 10MB max size, keep 3 rotated files
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "logs/mcgb_emu.log", 10 * 1024 * 1024, 3, false);
        
        auto logger = std::make_shared<spdlog::logger>("global_logger", file_sink);
        spdlog::set_default_logger(logger);
        spdlog::flush_on(spdlog::level::info);
    } catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Falha na inicializacao do Log: " << ex.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    setup_logger();
    // This pattern: [Timestamp] [Level] Message
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    
    spdlog::info("McGB Emulator was launch ______________________________________________________________________________________________________________");
    
    // 1. Start SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL failed to start: " << SDL_GetError() << std::endl;
        return 1;
    }

    // 2. Create Window
    spdlog::info("SDL inicializado com sucesso. Criando a janela...");
    SDL_Window* window = SDL_CreateWindow("McGB Window Test", WIDTH, HEIGHT, 0);
    if (!window) {
        std::cout << "Window failed to open: " << SDL_GetError() << std::endl;
        spdlog::critical("Falha ao criar a janela: {}", SDL_GetError());
        return 1;
    }

    // 3. Create Renderer
    spdlog::info("Janela criada com sucesso. Criando o renderer...");
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cout << "Renderer failed: " << SDL_GetError() << std::endl;
        spdlog::critical("Falha ao criar o renderer: {}", SDL_GetError());
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

    CPU A;
    A.execute(Instruction::InstructionWithImmediate(InstructionType::DEC,23));

    // 5. Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    spdlog::info("McGB Emulator was shutdown ____________________________________________________________________________________________________________\n");
    spdlog::shutdown(); // Libera todos os recursos do spdlog

    return 0;
}