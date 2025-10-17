//
// Created by Cold-Mint on 2025/10/9.
//
#include "App.h"
#include <SDL3/SDL.h>
#include "log/LogCat.h"
#include "Config.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
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
    LogCat::i("Initializing ImGui context...");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    LogCat::i("ImGui context created.");

    const ImGuiIO &io = ImGui::GetIO();
    (void) io;

    LogCat::i("Setting ImGui style to Light...");
    ImGui::StyleColorsLight();
    const auto fontPathOpt = appContext->resourcePackManager->getFontPath(
        appContext->config->mods.enabledResourcePack,
        *appContext->language);

    if (fontPathOpt.has_value()) {
        const std::string &fontPath = *fontPathOpt;
        if (io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 16.0F)) {
            LogCat::d("Loaded font: ", fontPath);
        } else {
            LogCat::e("Failed to load font (ImGui error): ", fontPath);
        }
    } else {
        LogCat::w("No font found for language '", *appContext->language, "', skipping font load");
    }


    LogCat::i("Initializing ImGui SDL3 backend for SDLRenderer...");
    if (!ImGui_ImplSDL3_InitForSDLRenderer(window, renderer)) {
        LogCat::e("ImGui_ImplSDL3_InitForSDLRenderer failed!");
        return false;
    }
    LogCat::i("ImGui SDL3 backend initialized successfully.");

    LogCat::i("Initializing ImGui SDLRenderer3 backend...");
    if (!ImGui_ImplSDLRenderer3_Init(renderer)) {
        LogCat::e("ImGui_ImplSDLRenderer3_Init failed!");
        return false;
    }
    LogCat::i("ImGui SDLRenderer3 backend initialized successfully.");
    return true;
}

void Glimmer::App::run() const {
    //The time interval of the target (in seconds)
    //目标的时间间隔（以秒为单位）
    const float targetFrameTime = 1.0F / appContext->config->window.framerate;
    //Target frame time (in milliseconds)
    //目标帧时间（毫秒为单位）
    const auto targetFrameTimeMs = static_cast<Uint32>(targetFrameTime * 1000.0F);
    Uint64 frameStart = SDL_GetTicks();
    float deltaTime = 0.0F;
    bool running = true;
    SDL_Event event;
    LogCat::i("Entering main loop...");
    appContext->sceneManager->changeScene(new SplashScene(appContext));
    appContext->sceneManager->setConsoleScene(new ConsoleScene(appContext));
    while (running) {
        Scene *scene = appContext->sceneManager->getScene();
        Scene *consoleScene = appContext->sceneManager->getConsoleScene();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                LogCat::i("Received SDL_QUIT event. Exiting...");
                running = false;
            } else {
                if (!consoleScene->HandleEvent(event)) {
                    // LogCat::d("ConsoleScene did not handle event, forwarding to current Scene...");
                    if (!scene->HandleEvent(event)) {
                        // LogCat::d("Scene did not handle event, forwarding to ImGui...");
                        ImGui_ImplSDL3_ProcessEvent(&event);
                    } else {
                        LogCat::w("Scene handled event, stopping propagation.");
                    }
                } else {
                    LogCat::w("ConsoleScene handled event, stopping propagation.");
                }
            }
        }
        consoleScene->Update(deltaTime);
        scene->Update(deltaTime);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        scene->Render(renderer);
        consoleScene->Render(renderer);
        SDL_RenderPresent(renderer);
        const auto frameTimeMs = SDL_GetTicks() - frameStart;
        frameStart = SDL_GetTicks();
        //Actual time interval (in seconds)
        //实际时间间隔（秒为单位）
        deltaTime = static_cast<float>(frameTimeMs) / 1000.0F;
        if (frameTimeMs < targetFrameTimeMs) {
            SDL_Delay(targetFrameTimeMs - frameTimeMs);
        }
    }
}
