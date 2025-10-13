//
// Created by Cold-Mint on 2025/10/9.
//
#include "App.h"
#include <SDL3/SDL.h>
#include "log/LogCat.h"
#include "Config.h"
#include "scene/SplashScene.h"
#include "scene/ConsoleScene.h"

bool Glimmer::App::init() {
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
        appContext->config->window.width,
        appContext->config->window.height,
        appContext->config->window.resizable ? SDL_WINDOW_RESIZABLE : SDL_WINDOW_FULLSCREEN
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
    //The time interval of the target (in seconds)
    //目标的时间间隔（以秒为单位）
    const float targetFrameTime = 1.0f / appContext->config->window.framerate;
    //Target frame time (in milliseconds)
    //目标帧时间（毫秒为单位）
    const float targetFrameTimeMs = targetFrameTime * 1000.0f;
    bool running = true;
    SDL_Event event;
    LogCat::i("Entering main loop...");
    appContext->sceneManager->changeScene(new SplashScene(appContext));
    appContext->sceneManager->setConsoleScene(new ConsoleScene(appContext));
    while (running) {
        const Uint64 frameStart = SDL_GetTicks();
        Scene *scene = appContext->sceneManager->getScene();
        Scene *consoleScene = appContext->sceneManager->getConsoleScene();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                LogCat::i("Received SDL_QUIT event. Exiting...");
                running = false;
            } else {
                scene->HandleEvent(event);
                consoleScene->HandleEvent(event);
            }
        }
        const auto frameTimeMs = static_cast<float>(SDL_GetTicks() - frameStart);
        //Actual time interval (in seconds)
        //实际时间间隔（秒为单位）
        const float deltaTime = frameTimeMs / 1000.0f;
        scene->Update(deltaTime);
        consoleScene->Update(deltaTime);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        scene->Render(renderer);
        consoleScene->Render(renderer);
        SDL_RenderPresent(renderer);
        if (frameTimeMs < targetFrameTimeMs) {
            SDL_Delay(static_cast<Uint32>(targetFrameTimeMs - frameTimeMs));
        }
    }
}
