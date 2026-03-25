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
    appContext_->GetResourcePackManager()->SetRenderer(renderer_, appContext_->GetPreloadColors()->error.accentColor,
                                                       appContext_->GetPreloadColors()->error.baseColor);
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
    auto preloadColors = appContext_->GetPreloadColors();
    const auto textColor = preloadColors->textColor;
    const auto textImColor = ImVec4(static_cast<float>(textColor.r) / 255,
                                    static_cast<float>(textColor.g) / 255,
                                    static_cast<float>(textColor.b) / 255,
                                    static_cast<float>(textColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_Text] = textImColor;
    const auto textDisabledColor = preloadColors->textDisabledColor;
    const auto textDisabledImColor = ImVec4(static_cast<float>(textDisabledColor.r) / 255,
                                            static_cast<float>(textDisabledColor.g) / 255,
                                            static_cast<float>(textDisabledColor.b) / 255,
                                            static_cast<float>(textDisabledColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_TextDisabled] = textDisabledImColor;
    const auto textLinkColor = preloadColors->textLinkColor;
    const auto textLinkImColor = ImVec4(static_cast<float>(textLinkColor.r) / 255,
                                        static_cast<float>(textLinkColor.g) / 255,
                                        static_cast<float>(textLinkColor.b) / 255,
                                        static_cast<float>(textLinkColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_TextLink] = textLinkImColor;
    const auto textSelectedBgColor = preloadColors->textSelectedBgColor;
    const auto textSelectedBgImColor = ImVec4(static_cast<float>(textSelectedBgColor.r) / 255,
                                              static_cast<float>(textSelectedBgColor.g) / 255,
                                              static_cast<float>(textSelectedBgColor.b) / 255,
                                              static_cast<float>(textSelectedBgColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg] = textSelectedBgImColor;
    const auto backgroundColor = preloadColors->backgroundColor;
    const auto backgroundImColor = ImVec4(static_cast<float>(backgroundColor.r) / 255,
                                          static_cast<float>(backgroundColor.g) / 255,
                                          static_cast<float>(backgroundColor.b) / 255,
                                          static_cast<float>(backgroundColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = backgroundImColor;
    const auto borderColor = preloadColors->borderColor;
    const auto borderImColor = ImVec4(static_cast<float>(borderColor.r) / 255,
                                      static_cast<float>(borderColor.g) / 255,
                                      static_cast<float>(borderColor.b) / 255,
                                      static_cast<float>(borderColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_Border] = borderImColor;
    const auto borderShadowColor = preloadColors->borderShadowColor;
    const auto borderShadowImColor = ImVec4(static_cast<float>(borderShadowColor.r) / 255,
                                            static_cast<float>(borderShadowColor.g) / 255,
                                            static_cast<float>(borderShadowColor.b) / 255,
                                            static_cast<float>(borderShadowColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_BorderShadow] = borderShadowImColor;
    const auto separatorColor = preloadColors->separatorColor;
    const auto separatorImColor = ImVec4(static_cast<float>(separatorColor.r) / 255,
                                         static_cast<float>(separatorColor.g) / 255,
                                         static_cast<float>(separatorColor.b) / 255,
                                         static_cast<float>(separatorColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_Separator] = separatorImColor;
    const auto separatorHoveredColor = preloadColors->separatorHoveredColor;
    const auto separatorHoveredImColor = ImVec4(static_cast<float>(separatorHoveredColor.r) / 255,
                                                static_cast<float>(separatorHoveredColor.g) / 255,
                                                static_cast<float>(separatorHoveredColor.b) / 255,
                                                static_cast<float>(separatorHoveredColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_SeparatorHovered] = separatorHoveredImColor;
    const auto separatorActiveColor = preloadColors->separatorActiveColor;
    const auto separatorActiveImColor = ImVec4(static_cast<float>(separatorActiveColor.r) / 255,
                                               static_cast<float>(separatorActiveColor.g) / 255,
                                               static_cast<float>(separatorActiveColor.b) / 255,
                                               static_cast<float>(separatorActiveColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_SeparatorActive] = separatorActiveImColor;
    const auto frameBgColor = preloadColors->frameBgColor;
    const auto frameBgImColor = ImVec4(static_cast<float>(frameBgColor.r) / 255,
                                       static_cast<float>(frameBgColor.g) / 255,
                                       static_cast<float>(frameBgColor.b) / 255,
                                       static_cast<float>(frameBgColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = frameBgImColor;
    const auto frameBgHoveredColor = preloadColors->frameBgHoveredColor;
    const auto frameBgHoveredImColor = ImVec4(static_cast<float>(frameBgHoveredColor.r) / 255,
                                              static_cast<float>(frameBgHoveredColor.g) / 255,
                                              static_cast<float>(frameBgHoveredColor.b) / 255,
                                              static_cast<float>(frameBgHoveredColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered] = frameBgHoveredImColor;
    const auto frameBgActiveColor = preloadColors->frameBgActiveColor;
    const auto frameBgActiveImColor = ImVec4(static_cast<float>(frameBgActiveColor.r) / 255,
                                             static_cast<float>(frameBgActiveColor.g) / 255,
                                             static_cast<float>(frameBgActiveColor.b) / 255,
                                             static_cast<float>(frameBgActiveColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive] = frameBgActiveImColor;
    const auto buttonColor = preloadColors->buttonColor;
    const auto buttonImColor = ImVec4(static_cast<float>(buttonColor.r) / 255,
                                      static_cast<float>(buttonColor.g) / 255,
                                      static_cast<float>(buttonColor.b) / 255,
                                      static_cast<float>(buttonColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_Button] = buttonImColor;
    const auto buttonHoveredColor = preloadColors->buttonHoveredColor;
    const auto buttonHoveredImColor = ImVec4(static_cast<float>(buttonHoveredColor.r) / 255,
                                             static_cast<float>(buttonHoveredColor.g) / 255,
                                             static_cast<float>(buttonHoveredColor.b) / 255,
                                             static_cast<float>(buttonHoveredColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = buttonHoveredImColor;
    const auto buttonActiveColor = preloadColors->buttonActiveColor;
    const auto buttonActiveImColor = ImVec4(static_cast<float>(buttonActiveColor.r) / 255,
                                            static_cast<float>(buttonActiveColor.g) / 255,
                                            static_cast<float>(buttonActiveColor.b) / 255,
                                            static_cast<float>(buttonActiveColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = buttonActiveImColor;
    const auto scrollbarBgColor = preloadColors->scrollbarBgColor;
    const auto scrollbarBgImColor = ImVec4(static_cast<float>(scrollbarBgColor.r) / 255,
                                           static_cast<float>(scrollbarBgColor.g) / 255,
                                           static_cast<float>(scrollbarBgColor.b) / 255,
                                           static_cast<float>(scrollbarBgColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_ScrollbarBg] = scrollbarBgImColor;
    const auto scrollbarGrabColor = preloadColors->scrollbarGrabColor;
    const auto scrollbarGrabImColor = ImVec4(static_cast<float>(scrollbarGrabColor.r) / 255,
                                             static_cast<float>(scrollbarGrabColor.g) / 255,
                                             static_cast<float>(scrollbarGrabColor.b) / 255,
                                             static_cast<float>(scrollbarGrabColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrab] = scrollbarGrabImColor;
    const auto scrollbarGrabHoveredColor = preloadColors->scrollbarGrabHoveredColor;
    const auto scrollbarGrabHoveredImColor = ImVec4(static_cast<float>(scrollbarGrabHoveredColor.r) / 255,
                                                    static_cast<float>(scrollbarGrabHoveredColor.g) / 255,
                                                    static_cast<float>(scrollbarGrabHoveredColor.b) / 255,
                                                    static_cast<float>(scrollbarGrabHoveredColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrabHovered] = scrollbarGrabHoveredImColor;
    const auto scrollbarGrabActiveColor = preloadColors->scrollbarGrabActiveColor;
    const auto scrollbarGrabActiveImColor = ImVec4(static_cast<float>(scrollbarGrabActiveColor.r) / 255,
                                                   static_cast<float>(scrollbarGrabActiveColor.g) / 255,
                                                   static_cast<float>(scrollbarGrabActiveColor.b) / 255,
                                                   static_cast<float>(scrollbarGrabActiveColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrabActive] = scrollbarGrabActiveImColor;
    const auto inputTextCursorColor = preloadColors->inputTextCursorColor;
    ImVec4 inputTextCursorImColor = ImVec4(static_cast<float>(inputTextCursorColor.r) / 255,
                                           static_cast<float>(inputTextCursorColor.g) / 255,
                                           static_cast<float>(inputTextCursorColor.b) / 255,
                                           static_cast<float>(inputTextCursorColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_InputTextCursor] = inputTextCursorImColor;
    const auto navCursorColor = preloadColors->navCursorColor;
    const auto navCursorImColor = ImVec4(static_cast<float>(navCursorColor.r) / 255,
                                         static_cast<float>(navCursorColor.g) / 255,
                                         static_cast<float>(navCursorColor.b) / 255,
                                         static_cast<float>(navCursorColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_NavCursor] = navCursorImColor;
    const auto headerColor = preloadColors->headerColor;
    const auto headerImColor = ImVec4(static_cast<float>(headerColor.r) / 255,
                                      static_cast<float>(headerColor.g) / 255,
                                      static_cast<float>(headerColor.b) / 255,
                                      static_cast<float>(headerColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_Header] = headerImColor;
    const auto headerHoveredColor = preloadColors->headerHoveredColor;
    const auto headerHoveredImColor = ImVec4(static_cast<float>(headerHoveredColor.r) / 255,
                                             static_cast<float>(headerHoveredColor.g) / 255,
                                             static_cast<float>(headerHoveredColor.b) / 255,
                                             static_cast<float>(headerHoveredColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered] = headerHoveredImColor;
    const auto headerActiveColor = preloadColors->headerActiveColor;
    const auto headerActiveImColor = ImVec4(static_cast<float>(headerActiveColor.r) / 255,
                                            static_cast<float>(headerActiveColor.g) / 255,
                                            static_cast<float>(headerActiveColor.b) / 255,
                                            static_cast<float>(headerActiveColor.a) / 255);
    ImGui::GetStyle().Colors[ImGuiCol_HeaderActive] = headerActiveImColor;
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

void glimmer::App::Run() {
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
                    bool handled = false;
                    for (const auto overlayScene: std::ranges::reverse_view(overlayScenes)) {
                        if (overlayScene->OnBackPressed()) {
                            handled = true;
                            break;
                        }
                    }
                    Scene *topScene = sceneManager->GetTopScene();
                    if (!handled && topScene != nullptr) {
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
