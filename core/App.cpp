//
// Created by Cold-Mint on 2025/10/9.
//
#include "App.h"

#include "log/LogCat.h"

bool Glimmer::App::init(const Config &config) {
    LogCat::i("Initializing SDL...");
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        LogCat::e("SDL_Init Error: ", SDL_GetError());
        return false;
    }
    initSDLSuccess = true;
    LogCat::i("SDL initialized successfully.");

    LogCat::i("Creating SDL window...");
    window = SDL_CreateWindow(
        "GlimmerWorks",
        config.window.width,
        config.window.height,
        config.window.resizable ? SDL_WINDOW_RESIZABLE : SDL_WINDOW_FULLSCREEN
    );
    if (!window) {
        LogCat::e("SDL_CreateWindow Error: ", SDL_GetError());
        return false;
    }
    LogCat::i("SDL window created successfully.");

    LogCat::i("Creating SDL renderer...");
    renderer =
            SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        LogCat::e("SDL_CreateRenderer Error: ", SDL_GetError());
        return false;
    }
    LogCat::i("SDL renderer created successfully.");
    return true;
}

void Glimmer::App::run() const {
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
}

