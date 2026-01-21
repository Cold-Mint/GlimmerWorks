//
// Created by Cold-Mint on 2025/10/9.
//
#include "App.h"
#include <SDL3/SDL.h>

#include <ranges>
#include "log/LogCat.h"
#include "Config.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "scene/SplashScene.h"
#include "scene/ConsoleOverlay.h"
#include "scene/DebugOverlay.h"
#include "SDL3_ttf/SDL_ttf.h"

bool glimmer::App::init() {
    LogCat::i("Initializing SDL...");
#ifdef __ANDROID__
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
    SDL_SetHint("SDL_ANDROID_TRAP_BACK_BUTTON", "1");
#endif
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        LogCat::e("SDL_Init Error: ", SDL_GetError());
        return false;
    }
    initSDLSuccess = true;
    LogCat::i("Initializing SDL_ttf...");
    if (!TTF_Init()) {
        LogCat::e("TTF_Init Error: ", SDL_GetError());
        return false;
    }
    initSDLTtfSuccess = true;
    LogCat::i("SDL initialized successfully.");

    LogCat::i("Creating SDL window...");
    Config *config = appContext->GetConfig();
    window = SDL_CreateWindow(
        "GlimmerWorks",
        config->window.width,
        config->window.height,
        config->window.resizable ? SDL_WINDOW_RESIZABLE : SDL_WINDOW_FULLSCREEN
    );
    if (window == nullptr) {
        LogCat::e("SDL_CreateWindow Error: ", SDL_GetError());
        return false;
    }
    LogCat::i("SDL window created successfully.");
    appContext->SetWindow(window);
    LogCat::i("Creating SDL renderer...");
    renderer_ =
            SDL_CreateRenderer(window, nullptr);
    if (renderer_ == nullptr) {
        LogCat::e("SDL_CreateRenderer Error: ", SDL_GetError());
        return false;
    }
    appContext->GetResourcePackManager()->SetRenderer(renderer_);
    // Enable alpha blending rendering
    // 启用按 alpha 混合渲染
    // This will allow us to use transparency during rendering
    // 这将允许我们在渲染时使用透明度
    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
    LogCat::i("SDL renderer created successfully.");
    LogCat::i("Initializing ImGui context...");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    LogCat::i("ImGui context created.");

    const ImGuiIO &io = ImGui::GetIO();
    (void) io;

    LogCat::i("Setting ImGui style to Light...");
    ImGui::StyleColorsLight();
    const auto fontPathOpt = appContext->GetResourcePackManager()->GetFontPath(
        config->mods.enabledResourcePack,
        appContext->GetLanguage());

    if (fontPathOpt.has_value()) {
        const std::string &fontPath = fontPathOpt.value();
        auto actualPath = appContext->GetVirtualFileSystem()->GetActualPath(fontPath);
        if (!actualPath.has_value()) {
            LogCat::e("An error occurred when converting to the actual font path.");
            return false;
        }
        if (io.Fonts->AddFontFromFileTTF(actualPath.value().c_str(), 16.0F)) {
            LogCat::d("Loaded font: ", fontPath);
        } else {
            LogCat::e("Failed to load font (ImGui error): ", fontPath);
        }
        if (TTF_Font *sdlFont = TTF_OpenFont(actualPath.value().c_str(), 16); !sdlFont) {
            LogCat::e("Failed to load SDL_ttf font: ", SDL_GetError());
        } else {
            LogCat::d("SDL_ttf font loaded: ", fontPath);
            appContext->SetFont(sdlFont);
        }
    } else {
        LogCat::w("No font found for language '", appContext->GetLanguage(), "', skipping font load");
    }


    LogCat::i("Initializing ImGui SDL3 backend for SDLRenderer...");
    if (!ImGui_ImplSDL3_InitForSDLRenderer(window, renderer_)) {
        LogCat::e("ImGui_ImplSDL3_InitForSDLRenderer failed!");
        return false;
    }
    LogCat::i("ImGui SDL3 backend initialized successfully.");

    LogCat::i("Initializing ImGui SDLRenderer3 backend...");
    if (!ImGui_ImplSDLRenderer3_Init(renderer_)) {
        LogCat::e("ImGui_ImplSDLRenderer3_Init failed!");
        return false;
    }
    LogCat::i("ImGui SDLRenderer3 backend initialized successfully.");
    AppContext::RestoreColorRenderer(renderer_);
    return true;
}

void glimmer::App::run() const {
    Uint64 frameStart = SDL_GetTicks();
    float deltaTime = 0.0F;
    SDL_Event event;
    LogCat::i("Entering main loop...");
    auto sceneManager = appContext->GetSceneManager();
    auto config = appContext->GetConfig();
    sceneManager->PushScene(std::make_unique<SplashScene>(appContext));
    sceneManager->AddOverlayScene(std::make_unique<ConsoleOverlay>(appContext));
    sceneManager->AddOverlayScene(std::make_unique<DebugOverlay>(appContext));
    auto &overlayScenes = sceneManager->GetOverlayScenes();
    while (appContext->Running() && sceneManager->GetSceneCount() > 0) {
        //The time interval of the target (in seconds)
        //目标的时间间隔（以秒为单位）
        const float targetFrameTime = 1.0F / config->window.framerate;
        //Target frame time (in milliseconds)
        //目标帧时间（毫秒为单位）
        const auto targetFrameTimeMs = static_cast<Uint32>(targetFrameTime * 1000.0F);
        for (const auto overlayScene: std::ranges::reverse_view(overlayScenes)) {
            overlayScene->OnFrameStart();
        }
        appContext->ProcessMainThreadTasks();
        if (Scene *topScene = sceneManager->GetTopScene(); topScene != nullptr) {
            topScene->OnFrameStart();
        }
        while (SDL_PollEvent(&event)) {
#ifdef __ANDROID__
            if (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.key == SDLK_AC_BACK) {
                if (Scene *topScene = sceneManager->GetTopScene(); topScene != nullptr) {
                    if (!topScene->OnBackPressed()) {
                        sceneManager->PopScene();
                        break;
                    }
                }
            }
#else
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE && !event.key.repeat) {
                    if (Scene *topScene = sceneManager->GetTopScene(); topScene != nullptr) {
                        if (!topScene->OnBackPressed()) {
                            sceneManager->PopScene();
                            break;
                        }
                    }
                }
            }
#endif

            if (event.type == SDL_EVENT_QUIT) {
                LogCat::i("Received SDL_QUIT event. Exiting...");
                appContext->ExitApp();
            } else {
                bool handled = false;
                for (const auto overlayScene: std::ranges::reverse_view(overlayScenes)) {
                    if (overlayScene->HandleEvent(event)) {
                        handled = true;
                        LogCat::w("OverlayScene handled event, stopping propagation.");
                        break;
                    }
                }
                if (!handled) {
                    if (Scene *topScene = sceneManager->GetTopScene(); topScene != nullptr) {
                        if (topScene->
                            HandleEvent(event)) {
                            handled = true;
                            LogCat::i("Main scene handled event, stopping propagation.");
                        }
                    }
                }
                if (!handled) {
                    ImGui_ImplSDL3_ProcessEvent(&event);
                }
            }
        }
        ImGui_ImplSDL3_NewFrame();
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui::NewFrame();
        for (const auto overlay: overlayScenes) {
            overlay->Update(deltaTime);
        }
        if (Scene *topScene = sceneManager->GetTopScene(); topScene != nullptr) {
            topScene->Update(deltaTime);
        }

        SDL_RenderClear(renderer_);
#if  defined(NDEBUG)
        if (Scene *topScene = sceneManager->GetTopScene(); topScene != nullptr) {
            topScene->Render(renderer_);
        }
        for (const auto overlay: overlayScenes) {
            overlay->Render(renderer_);
        }
#else
        SDL_Color oldColor;
        SDL_GetRenderDrawColor(renderer_, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);
        if (Scene *topScene = sceneManager->GetTopScene(); topScene != nullptr) {
            topScene->Render(renderer_);
        }
        SDL_Color newColor;
        SDL_GetRenderDrawColor(renderer_, &newColor.r, &newColor.g, &newColor.b, &newColor.a);
        if (oldColor.a != newColor.a || oldColor.r != newColor.r || oldColor.g != newColor.g || oldColor.b != newColor.
            b) {
            LogCat::e("The color of the renderer has been changed by the scene.");
            assert(false);
        }
        for (const auto overlay: overlayScenes) {
            SDL_GetRenderDrawColor(renderer_, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);
            overlay->Render(renderer_);
            SDL_Color overlayColor;
            SDL_GetRenderDrawColor(renderer_, &overlayColor.r, &overlayColor.g, &overlayColor.b, &overlayColor.a);
            if (oldColor.a != overlayColor.a || oldColor.r != overlayColor.r || oldColor.g != overlayColor.g || oldColor
                .b !=
                overlayColor.b) {
                LogCat::e("The color of the renderer has been changed by the overlay scene.");
                assert(false);
            }
        }


#endif
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer_);
        SDL_RenderPresent(renderer_);
        const auto frameEnd = SDL_GetTicks();
        const auto frameTimeMs = frameEnd - frameStart;
        if (frameTimeMs < targetFrameTimeMs) {
            SDL_Delay(targetFrameTimeMs - frameTimeMs);
        }
        const auto actualFrameEnd = SDL_GetTicks();
        deltaTime = static_cast<float>(actualFrameEnd - frameStart) / 1000.0F;
        frameStart = actualFrameEnd;
    }
}
