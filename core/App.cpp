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
#include <SDL3/SDL_init.h>

void glimmer::App::RendererUiMessage() {
    auto &uiMessages = appContext_->GetGameUIMessages();
    uint64_t now = SDL_GetTicks();
    int32_t delta = now - lastTime_;
    lastTime_ = now;
    //Delete expired messages
    //删除过期的消息
    std::erase_if(uiMessages,
                  [now](const GameUIMessage &msg) {
                      return msg.expireTime <= now;
                  });

    //Get window size
    //获取窗口尺寸
    int windowW = 0;
    int windowH = 0;
    SDL_GetRenderOutputSize(renderer_, &windowW, &windowH);

    constexpr float padding = 16.0f;
    constexpr float spacing = 6.0f;

    //First round: Update tween + Calculate total height
    //第一遍：更新 tween + 计算总高度
    float totalHeight = 0.0f;

    for (auto &msg: uiMessages) {
        msg.tween.step(delta);
        msg.alpha = msg.tween.peek();

        if (msg.alpha <= 0.01f) {
            continue;
        }

        SDL_Surface *surface = TTF_RenderText_Blended(
            appContext_->GetFont(),
            msg.text.c_str(),
            msg.text.length(),
            {255, 255, 255, 255}
        );

        if (!surface) {
            continue;
        }

        totalHeight += static_cast<float>(surface->h) + spacing;

        SDL_DestroySurface(surface);
    }

    if (!uiMessages.empty()) {
        totalHeight -= spacing;
    }

    float startY = windowH - totalHeight - padding;

    // Second round: Actual drawing
    // 第二遍：真正绘制
    for (auto &msg: uiMessages) {
        if (msg.alpha <= 0.01f)
            continue;

        SDL_Surface *surface = TTF_RenderText_Blended(
            appContext_->GetFont(),
            msg.text.c_str(),
            msg.text.length(),
            {255, 255, 255, 255}
        );

        if (!surface)
            continue;

        // ⭐ 先保存宽高（避免 use-after-free）
        float w = static_cast<float>(surface->w);
        float h = static_cast<float>(surface->h);

        SDL_Texture *texture =
                SDL_CreateTextureFromSurface(renderer_, surface);

        SDL_DestroySurface(surface);

        if (!texture)
            continue;

        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

        Uint8 alpha =
                static_cast<Uint8>(msg.alpha * 255.0f);

        SDL_SetTextureAlphaMod(texture, alpha);

        SDL_FRect dst = {
            padding,
            startY,
            w,
            h
        };

        SDL_RenderTexture(renderer_, texture, nullptr, &dst);

        startY += h + spacing;

        SDL_DestroyTexture(texture);
    }
}

glimmer::App::~App() {
    LogCat::i("Destroy the app");
    if (window != nullptr) {
        SDL_DestroyWindow(window);
    }
    if (initSDLTtfSuccess) {
        TTF_Quit();
    }
    if (initSDLSuccess) {
        SDL_Quit();
    }
}

glimmer::App::App(AppContext *ac) : appContext_(ac) {
    window = nullptr;
    renderer_ = nullptr;
    initSDLSuccess = false;
    initSDLTtfSuccess = false;
}

bool glimmer::App::Init() {
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
    Config *config = appContext_->GetConfig();
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
    appContext_->SetWindow(window);
    LogCat::i("Creating SDL renderer...");
    renderer_ =
            SDL_CreateRenderer(window, nullptr);
    if (renderer_ == nullptr) {
        LogCat::e("SDL_CreateRenderer Error: ", SDL_GetError());
        return false;
    }
    SDL_SetRenderVSync(renderer_, config->window.vSync);
    appContext_->SetRenderer(renderer_);
    appContext_->GetResourcePackManager()->SetRenderer(renderer_);
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

    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr;
    (void) io;

    LogCat::i("Setting ImGui style to Light...");
    ImGui::StyleColorsLight();
    const auto fontPathOpt = appContext_->GetResourcePackManager()->GetFontPath(
        config->mods.enabledResourcePack,
        appContext_->GetLanguage());

    if (fontPathOpt.has_value()) {
        const std::string &fontPath = fontPathOpt.value();
        if (!appContext_->GetVirtualFileSystem()->Exists(fontPath)) {
            return false;
        }
        auto actualPath = appContext_->GetVirtualFileSystem()->GetActualPath(fontPath);
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
            appContext_->SetFont(sdlFont);
        }
    } else {
        LogCat::w("No font found for language '", appContext_->GetLanguage(), "', skipping font load");
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

void glimmer::App::Run()  {
    Uint64 frameStart = SDL_GetTicks();
    float deltaTime = 0.0F;
    SDL_Event event;
    LogCat::i("Entering main loop...");
    auto sceneManager = appContext_->GetSceneManager();
    auto config = appContext_->GetConfig();
    sceneManager->PushScene(std::make_unique<SplashScene>(appContext_));
    sceneManager->AddOverlayScene(std::make_unique<ConsoleOverlay>(appContext_));
    sceneManager->AddOverlayScene(std::make_unique<DebugOverlay>(appContext_));
    auto &overlayScenes = sceneManager->GetOverlayScenes();
    while (appContext_->Running() && sceneManager->GetSceneCount() > 0) {
        //The time interval of the target (in seconds)
        //目标的时间间隔（以秒为单位）
        const float targetFrameTime = 1.0F / config->window.framerate;
        //Target frame time (in milliseconds)
        //目标帧时间（毫秒为单位）
        const auto targetFrameTimeMs = static_cast<Uint32>(targetFrameTime * 1000.0F);
        for (const auto overlayScene: std::ranges::reverse_view(overlayScenes)) {
            overlayScene->OnFrameStart();
        }
        appContext_->ProcessMainThreadTasks();
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
                if (event.key.key == SDLK_F2 && !event.key.repeat) {
                    appContext_->CreateScreenshot();
                }
            }
#endif

            if (event.type == SDL_EVENT_QUIT) {
                LogCat::i("Received SDL_QUIT event. Exiting...");
                appContext_->ExitApp();
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
        RendererUiMessage();
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
