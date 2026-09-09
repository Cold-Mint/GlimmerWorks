/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "core/app/App.h"
#include <SDL3/SDL.h>

#include <ranges>
#include "core/log/LogCat.h"
#include "core/config/Config.h"
#include "core/scene/SplashScene.h"
#if  !defined(NDEBUG)
#include "core/scene/DebugOverlay.h"
#endif
#include "core/app/AppEventLoop.h"
#include "core/app/AppRenderer.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "SDL3/SDL_init.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "core/gpu/GpuShaderCompiler.h"
#include "core/scene/SceneManager.h"
#include "core/console/ConsoleWorker.h"
#include "core/console/hook/CommandHookManager.h"
#include "RmlUi/Core/Context.h"
#include "core/scene/ConsoleOverlay.h"
#include "core/scene/UIMessageOverlay.h"
#include "core/tick/TickWorker.h"


bool glimmer::App::InitSDL() {
#ifdef __ANDROID__
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
    SDL_SetHint("SDL_ANDROID_TRAP_BACK_BUTTON", "1");
#endif
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        LogCat::e(std::source_location::current(), "sdl_init_failed", "SDL_Init failed");
        return false;
    }
    initSDLSuccess_ = true;
    LogCat::i("sdl_init_succeeded", "SDL_Init succeeded");
    if (!MIX_Init()) {
        LogCat::e(std::source_location::current(), "mix_init_failed", "MIX_Init failed");
        return false;
    }
    initSDLMixSuccess_ = true;
    LogCat::i("mix_init_succeeded", "MIX_Init succeeded");

    if (!TTF_Init()) {
        LogCat::e(std::source_location::current(), "ttf_init_failed", "TTF_Init failed");
        return false;
    }
    initSDLTtfSuccess_ = true;
    LogCat::i("ttf_init_succeeded", "TTF_Init succeeded");
    return true;
}

bool glimmer::App::InitWindowAndRenderer() const {
    Config *config = appContext_->GetConfig();
    if (config == nullptr) {
        LogCat::e(std::source_location::current(), "config_is_null", "config is nullptr");
        return false;
    }
    LogCat::i("creating_window", "Creating window: width={}, height={}, fullscreen={}", config->window.width,
              config->window.height, config->window.fullscreen);
    WindowContext *windowContext = appContext_->GetWindowContext();
    if (windowContext == nullptr) {
        LogCat::e(std::source_location::current(), "window_context_is_null", "windowContext is nullptr");
        return false;
    }
    if (!windowContext->CreateWindowAndDevice(config->window.width, config->window.height, config->window.fullscreen)) {
        return false;
    }
    ResourcePackManager *resourcePackManager = appContext_->GetResourcePackManager();
    if (resourcePackManager == nullptr) {
        LogCat::e(std::source_location::current(), "resource_pack_manager_is_null", "ResourcePackManager is nullptr");
        return false;
    }
    GpuShaderCompiler::Init();
    LogCat::i("gpu_renderer_created", "GpuRenderer created successfully");
    RmlContext *rmlContext = appContext_->GetRmlContext();
    if (rmlContext == nullptr) {
        LogCat::e(std::source_location::current(), "rml_context_is_null", "RmlContext is nullptr");
        return false;
    }
    LogCat::i("initializing_rml_context", "Initializing RmlContext");
    rmlContext->Init(appContext_->GetVirtualFileSystem(), windowContext->GetDevice(),
                     appContext_->GetResourceLocator(), appContext_->GetLangsValue(), windowContext->GetWindow(),
                     config->window.width,
                     config->window.height);
    LogCat::i("rml_context_initialized", "RmlContext initialized successfully");
    LogCat::i("resource_pack_manager_gpu_context_set", "ResourcePackManager GPU context set");
    LogCat::i("init_window_and_renderer_completed", "InitWindowAndRenderer completed successfully");
    return true;
}

bool glimmer::App::InitFont() const {
    const Config *config = appContext_->GetConfig();
    if (config == nullptr) {
        LogCat::e(std::source_location::current(), "config_is_null", "config is nullptr");
        return false;
    }
    ResourcePackManager *resourcePackManager = appContext_->GetResourcePackManager();
    if (resourcePackManager == nullptr) {
        LogCat::e(std::source_location::current(), "resource_pack_manager_is_null", "resourcePackManager is nullptr");
        return false;
    }
    LogCat::i("loading_font_for_language", "Loading font for language: {}", appContext_->GetLanguage());
    const auto fontPathOpt = resourcePackManager->GetFontPath(
        config->mods.enabledResourcePack,
        appContext_->GetLanguage(),
        appContext_->GetVirtualFileSystem());

    if (!fontPathOpt.has_value()) {
        LogCat::i("no_font_configured", "No font configured, skipping font initialization");
        return true;
    }

    const std::filesystem::path &fontPath = fontPathOpt.value();
    LogCat::i("font_path", "Font path: {}", fontPath.string());
    const VirtualFileSystem *virtualFileSystem = appContext_->GetVirtualFileSystem();
    if (virtualFileSystem == nullptr) {
        LogCat::e(std::source_location::current(), "vfs_is_null", "virtualFileSystem is nullptr");
        return false;
    }
    if (!virtualFileSystem->Exists(fontPath)) {
        LogCat::w(std::source_location::current(), "font_file_not_found", "Font file not found: {}", fontPath.string());
        return false;
    }

    RmlContext *rmlContext = appContext_->GetRmlContext();
    if (rmlContext == nullptr) {
        LogCat::e(std::source_location::current(), "rml_context_is_null", "rmlContext is nullptr");
        return false;
    }
    if (!rmlContext->LoadFont(virtualFileSystem, fontPath)) {
        LogCat::e(std::source_location::current(), "rml_context_load_font_failed", "RmlContext Failed to load font: {}",
                  fontPath.string());
        return false;
    }
    LogCat::i("font_loaded", "Font loaded successfully: {}", fontPath.string());
    return true;
}

bool glimmer::App::InitAudio() {
    Config *config = appContext_->GetConfig();
    if (config == nullptr) {
        LogCat::e(std::source_location::current(), "config_is_null", "config is nullptr");
        return false;
    }
    SDL_AudioSpec audioSpec;
    const std::string &audioFormat = config->audio.format;
    if (audioFormat == "U8") {
        audioSpec.format = SDL_AUDIO_U8;
    } else if (audioFormat == "S16") {
        audioSpec.format = SDL_AUDIO_S16;
    } else if (audioFormat == "S32") {
        audioSpec.format = SDL_AUDIO_S32;
    } else {
        audioSpec.format = SDL_AUDIO_F32;
    }

    audioSpec.channels = config->audio.channels;
    audioSpec.freq = config->audio.freq;
    LogCat::i("creating_audio_mixer", "Creating audio mixer: format={}, channels={}, freq={}", audioFormat,
              config->audio.channels, config->audio.freq);

    mixer_ = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec);
    if (mixer_ == nullptr) {
        LogCat::e(std::source_location::current(), "mix_create_mixer_device_failed", "MIX_CreateMixerDevice failed");
        return false;
    }
    LogCat::i("audio_mixer_created", "Audio mixer created successfully");

    ResourcePackManager *resourcePackManager = appContext_->GetResourcePackManager();
    if (resourcePackManager == nullptr) {
        LogCat::e(std::source_location::current(), "resource_pack_manager_is_null", "resourcePackManager is nullptr");
        return false;
    }
    // resourcePackManager->SetMixer(mixer_);
    LogCat::i("loading_main_menu_bgm", "Loading main menu BGM");
    AudioContext *audioContext = appContext_->GetAudioContext();
    if (audioContext == nullptr) {
        LogCat::e(std::source_location::current(), "audio_context_is_null", "audioContext is nullptr");
        return false;
    }
    ResourceLocator *resourceLocator = appContext_->GetResourceLocator();
    if (resourceLocator == nullptr) {
        LogCat::e(std::source_location::current(), "resource_locator_is_null", "resourceLocator is nullptr");
        return false;
    }
    audioContext->LoadMainMenuBGM(resourceLocator);
    AudioManager *audioManager = audioContext->GetAudioManager();
    if (audioManager == nullptr) {
        LogCat::e(std::source_location::current(), "audio_manager_is_null", "audioManager is nullptr");
        return false;
    }
    audioManager->SetMixer(mixer_);

    LogCat::i("configuring_audio_tracks", "Configuring audio tracks: count={}", config->audio.track.size());
    for (const AudioTrack &trackConfig: config->audio.track) {
        audioManager->CreateTracks(trackConfig.type, trackConfig.trackCount);
        audioManager->SetTypeVolume(trackConfig.type, trackConfig.volume);
        LogCat::i("audio_track", "  Track: type={}, count={}, volume={}",
                  static_cast<int>(std::to_underlying(trackConfig.type)),
                  trackConfig.trackCount,
                  trackConfig.volume);
    }
    audioManager->SetMasterVolume(config->audio.masterVolume);
    LogCat::i("master_volume_set", "Master volume set to: {}", config->audio.masterVolume);

    LogCat::i("init_audio_completed", "InitAudio completed successfully");
    return true;
}

bool glimmer::App::CheckWindowSizeChange(WindowContext *windowContext, const int &windowWidth,
                                         const int &windowHeight) {
    bool changed = false;
    if (windowHeight != windowContext->GetWindowHeight()) {
        changed = true;
        windowContext->SetWindowHeight(windowHeight);
    }
    if (windowWidth != windowContext->GetWindowWidth()) {
        changed = true;
        windowContext->SetWindowWidth(windowWidth);
    }
    return changed;
}

glimmer::App::~App() {
    GpuShaderCompiler::Shutdown();
    if (tickWorker_ != nullptr) {
        tickWorker_->RemoveCallback(this);
    }
    if (appContext_ != nullptr) {
        if (WindowContext *windowContext = appContext_->GetWindowContext(); windowContext != nullptr) {
            windowContext->Shutdown();
        }
    }
    if (initSDLMixSuccess_) {
        MIX_Quit();
    }
    if (initSDLTtfSuccess_) {
        TTF_Quit();
    }
    if (initSDLSuccess_) {
        SDL_Quit();
    }
}

glimmer::App::App(AppContext *appContext) : appContext_(appContext) {
    tickWorker_ = appContext_->GetTickWorker();
    tickWorker_->AddCallback(this);
    sceneManager_ = appContext_->GetSceneManager();
}

bool glimmer::App::Init() {
    LogCat::i("starting_app_initialization", "Starting application initialization");
    const bool result = InitSDL() &&
                        InitWindowAndRenderer() &&
                        InitFont() &&
                        InitAudio();
    if (result) {
        LogCat::i("app_initialization_completed", "Application initialization completed successfully");
    } else {
        LogCat::e(std::source_location::current(), "app_initialization_failed", "Application initialization failed");
    }
    return result;
}

void glimmer::App::Run() const {
    LogCat::i("starting_app_main_loop", "Starting application main loop");
    const auto config = appContext_->GetConfig();

    LogCat::i("initializing_scenes_and_console", "Initializing scenes and console");
    InitScenesAndConsole();

    Uint64 frameStart = SDL_GetTicks();
    Uint64 lastInputTime = SDL_GetTicks();
    float deltaTime = 0.0F;
    uint64_t configFingerprint = config->GetFingerprint() + 1;

    LogCat::i("creating_event_loop_and_renderer", "Creating event loop and renderer");
    AppEventLoop eventLoop(appContext_, lastInputTime);
    AppRenderer renderer(appContext_);

    WindowContext *windowContext = appContext_->GetWindowContext();
    if (windowContext == nullptr) {
        LogCat::e(std::source_location::current(), "window_context_is_null", "windowContext is nullptr");
        return;
    }
    MainThreadDispatcher *mainThreadDispatcher = appContext_->GetMainThreadDispatcher();
    if (mainThreadDispatcher == nullptr) {
        LogCat::e(std::source_location::current(), "main_thread_dispatcher_is_null", "mainThreadDispatcher is nullptr");
        return;
    }
    RmlContext *rmlContext = appContext_->GetRmlContext();
    if (rmlContext == nullptr) {
        LogCat::e(std::source_location::current(), "rml_context_is_null", "rmlContext is nullptr");
        return;
    }
    Rml::Context *rmlContextCore = rmlContext->GetRmlContext();
    LogCat::i("entering_main_game_loop", "Entering main game loop");
    int windowWidth = 0;
    int windowHeight = 0;
    SDL_Window *window = windowContext->GetWindow();
    if (window == nullptr) {
        LogCat::e(std::source_location::current(), "window_is_null", "window is nullptr");
        return;
    }
    while (appContext_->IsRunning()) {
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);
        if (CheckWindowSizeChange(windowContext, windowWidth, windowHeight)) {
            LogCat::i("window_size_changed", "Window size changed: {}x{}", windowWidth, windowHeight);
            if (rmlContextCore != nullptr) {
                rmlContextCore->SetDimensions({windowWidth, windowHeight});
            }
            HandleWindowSizeChange(windowWidth, windowHeight);
        }
        if (CheckConfigChange(configFingerprint)) {
            LogCat::i("configuration_changed", "Configuration changed, reloading hooks and scenes");
        }
        const float targetFrameTime = CalculateTargetFrameTime(frameStart, lastInputTime);
        const auto targetFrameTimeMs = static_cast<Uint32>(targetFrameTime * 1000.0F);
        NotifyFrameStart();
        mainThreadDispatcher->ProcessMainThreadTasks();
        eventLoop.ProcessEvents(frameStart);
        rmlContext->UpdateContext();
        UpdateScenes(deltaTime);
        renderer.RenderFrame(windowWidth, windowHeight);
        const Uint64 frameTimeMs = SDL_GetTicks() - frameStart;
        if (frameTimeMs < targetFrameTimeMs) {
            SDL_Delay(targetFrameTimeMs - frameTimeMs);
        }
        const Uint64 actualFrameEnd = SDL_GetTicks();
        deltaTime = static_cast<float>(actualFrameEnd - frameStart) / 1000.0F;
        frameStart = actualFrameEnd;
    }
    LogCat::i("main_game_loop_exited", "Main game loop exited");
}

void glimmer::App::HandleWindowSizeChange(const int &windowWidth, const int &windowHeight) const {
    const auto &overlayScenes = sceneManager_->GetOverlayScenes();
    for (const auto overlayScene: std::ranges::reverse_view(overlayScenes)) {
        overlayScene->OnWindowSizeChanged(windowWidth, windowHeight);
    }
    if (Scene *topScene = sceneManager_->GetTopScene(); topScene != nullptr) {
        topScene->OnWindowSizeChanged(windowWidth, windowHeight);
    }
}

float glimmer::App::CalculateTargetFrameTime(const uint64_t frameStart, const uint64_t lastInputTime) const {
    const auto config = appContext_->GetConfig();
    const float idleDelay = config->window.idleDelay;

    if (idleDelay == -1) {
        return 1.0F / config->window.normalTargetFps;
    }

    if (static_cast<float>(frameStart - lastInputTime) * 0.001F < idleDelay) {
        return 1.0F / config->window.normalTargetFps;
    }
    return 1.0F / config->window.idleTargetFps;
}

bool glimmer::App::CheckConfigChange(uint64_t &configFingerprint) const {
    const auto config = appContext_->GetConfig();
    const uint64_t nowConfigFingerprint = config->GetFingerprint();
    if (configFingerprint == nowConfigFingerprint) {
        return false;
    }

    if (CommandHookManager *commandHookManager = appContext_->GetConsoleContext()->GetCommandHookManager();
        commandHookManager != nullptr) {
        commandHookManager->LoadHookFromConfig(config->commandHooks);
    }

    const auto &overlayScenes = sceneManager_->GetOverlayScenes();
    for (const auto overlayScene: std::ranges::reverse_view(overlayScenes)) {
        overlayScene->OnConfigChanged(config);
    }
    if (Scene *topScene = sceneManager_->GetTopScene(); topScene != nullptr) {
        topScene->OnConfigChanged(config);
    }

    configFingerprint = nowConfigFingerprint;
    return true;
}

void glimmer::App::NotifyFrameStart() const {
    const auto &overlayScenes = sceneManager_->GetOverlayScenes();
    for (const auto overlayScene: std::ranges::reverse_view(overlayScenes)) {
        overlayScene->OnFrameStart();
    }
    if (Scene *topScene = sceneManager_->GetTopScene(); topScene != nullptr) {
        topScene->OnFrameStart();
    }
}

void glimmer::App::UpdateScenes(const float deltaTime) const {
    const auto &overlayScenes = sceneManager_->GetOverlayScenes();
    for (const auto overlay: overlayScenes) {
        overlay->Update(deltaTime);
    }
    if (Scene *topScene = sceneManager_->GetTopScene(); topScene != nullptr) {
        topScene->Update(deltaTime);
    }
}

void glimmer::App::InitScenesAndConsole() const {
    auto sceneManager = appContext_->GetSceneManager();
    sceneManager->PushScene(std::make_unique<SplashScene>(appContext_));
    // #if  !defined(NDEBUG)
    //     sceneManager->AddOverlayScene(std::make_unique<DebugOverlay>(appContext_));
    // #endif
    sceneManager->AddOverlayScene(std::make_unique<ConsoleOverlay>(appContext_));
    sceneManager->AddOverlayScene(std::make_unique<UIMessageOverlay>(appContext_));
    ConsoleWorker *consoleWorker = appContext_->GetConsoleContext()->GetConsoleWorker();
    if (consoleWorker == nullptr) {
        return;
    }
    consoleWorker->PushOnMessage(
        std::make_unique<std::function<void(const std::string &)> >([this](const std::string &text) {
            // if (appContext_ == nullptr) {
            //     return;
            // }
            // appContext_->AddUIMessage(text);
        })
    );
}

void glimmer::App::OnTick(const uint64_t tick) {
    const auto &overlayScenes = sceneManager_->GetOverlayScenes();
    for (const auto overlay: overlayScenes) {
        overlay->OnTick(tick);
    }
    if (Scene *topScene = sceneManager_->GetTopScene(); topScene != nullptr) {
        topScene->OnTick(tick);
    }
}
