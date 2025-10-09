#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <SDL3/SDL.h>

#include "core/Config.h"
#include "core/log/LogCat.h"

using namespace Glimmer;


int main() {
    LogCat::i("Starting GlimmerWorks...");
    Config &config = Config::getInstance();
    LogCat::i("Loading config.json...");
    if (!config.loadConfig("config.json")) {
        LogCat::e("Failed to load config.json");
        return 1;
    }
    LogCat::i("The config.json load was successful.");

    LogCat::i("Initializing SDL...");
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        LogCat::e("SDL_Init Error: ", SDL_GetError());
        return 1;
    }
    LogCat::i("SDL initialized successfully.");

    LogCat::i("Creating SDL window...");
    SDL_Window *window = SDL_CreateWindow(
        "GlimmerWorks",
        config.window.width,
        config.window.height,
        config.window.resizable ? SDL_WINDOW_RESIZABLE : SDL_WINDOW_FULLSCREEN
    );
    if (!window) {
        LogCat::e("SDL_CreateWindow Error: ", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    LogCat::i("SDL window created successfully.");

    LogCat::i("Creating SDL renderer...");
    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        LogCat::e("SDL_CreateRenderer Error: ", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    LogCat::i("SDL renderer created successfully.");

    bool running = true;
    SDL_Event event;

    LogCat::i("Entering main loop...");
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                LogCat::i("Received SDL_QUIT event. Exiting...");
                running = false;
            }
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // RGBA
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    LogCat::i("Cleaning up SDL resources...");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    LogCat::i("SDL cleaned up. Exiting program.");

    return 0;
}
