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
#include "App.h"
#include <SDL3/SDL.h>

#include <ranges>
#include "log/LogCat.h"
#include "Config.h"
#include "scene/SplashScene.h"
#if  !defined(NDEBUG)
#include "scene/DebugOverlay.h"
#endif
#include "AppEventLoop.h"
#include "AppRenderer.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "utils/ColorUtils.h"
#include "scene/SceneManager.h"
#include "console/ConsoleWorker.h"
#include "CommandHookManager.h"


bool glimmer::App::InitSDL()
{
#ifdef __ANDROID__
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
    SDL_SetHint("SDL_ANDROID_TRAP_BACK_BUTTON", "1");
#endif
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        return false;
    }
    initSDLSuccess_ = true;
    if (!MIX_Init())
    {
        return false;
    }
    initSDLMixSuccess_ = true;

    if (!TTF_Init())
    {
        return false;
    }
    initSDLTtfSuccess_ = true;
    return true;
}

bool glimmer::App::InitWindowAndRenderer()
{
    Config* config = appContext_->GetConfig();
    if (config == nullptr)
    {
        LogCat::e(std::source_location::current(), "config is nullptr");
        return false;
    }
    window = SDL_CreateWindow(
        "GlimmerWorks",
        config->window.width,
        config->window.height,
        config->window.fullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_RESIZABLE
    );
    if (window == nullptr)
    {
        LogCat::e(std::source_location::current(), "window is nullptr");
        return false;
    }

    WindowContext* windowContext = appContext_->GetWindowContext();
    if (windowContext == nullptr)
    {
        LogCat::e(std::source_location::current(), "windowContext is nullptr");
        return false;
    }
    windowContext->SetWindow(window);
    renderer_ = SDL_CreateRenderer(window, nullptr);
    if (renderer_ == nullptr)
    {
        LogCat::e(std::source_location::current(), "renderer_ is nullptr");
        return false;
    }
#if  !defined(NDEBUG)
    const SDL_PropertiesID rendererProperties = SDL_GetRendererProperties(renderer_);
    const char* driverName = SDL_GetStringProperty(rendererProperties, SDL_PROP_RENDERER_NAME_STRING, nullptr);
    LogCat::i("driverName = ", driverName);
#endif
    SDL_SetRenderVSync(renderer_, config->window.vSync);
    windowContext->SetRenderer(renderer_);
    RmlContext* rmlContext = appContext_->GetRmlContext();
    if (rmlContext == nullptr)
    {
        LogCat::e(std::source_location::current(), "RmlContext is nullptr");
        return false;
    }
    rmlContext->Init(renderer_, config->window.width,
                     config->window.height);
    ResourcePackManager* resourcePackManager = appContext_->GetResourcePackManager();
    if (resourcePackManager == nullptr)
    {
        LogCat::e(std::source_location::current(), "ResourcePackManager is nullptr");
        return false;
    }
    resourcePackManager->SetRenderer(renderer_, appContext_->GetGraphicsContext()->GetPreloadColors());

    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
    SDL_SetDefaultTextureScaleMode(renderer_, SDL_SCALEMODE_PIXELART);
    return true;
}

bool glimmer::App::InitFont() const
{
    const Config* config = appContext_->GetConfig();
    if (config == nullptr)
    {
        return false;
    }
    ResourcePackManager* resourcePackManager = appContext_->GetResourcePackManager();
    if (resourcePackManager == nullptr)
    {
        return false;
    }
    const auto fontPathOpt = resourcePackManager->GetFontPath(
        config->mods.enabledResourcePack,
        appContext_->GetLanguage());

    if (!fontPathOpt.has_value())
    {
        return true;
    }

    const std::filesystem::path& fontPath = fontPathOpt.value();
    const VirtualFileSystem* virtualFileSystem = appContext_->GetVirtualFileSystem();
    if (virtualFileSystem == nullptr)
    {
        return false;
    }
    if (!virtualFileSystem->Exists(fontPath))
    {
        return false;
    }

    auto actualPath = virtualFileSystem->GetActualPath(fontPath);
    if (!actualPath.has_value())
    {
        return false;
    }
    if (!RmlContext::LoadFont(std::filesystem::path(actualPath.value().c_str())))
    {
        LogCat::e(std::source_location::current(), "RmlContext Failed to load font: ", actualPath.value());
        return false;
    }
    TTF_Font* sdlFont = TTF_OpenFont(actualPath.value().c_str(), 16);
    if (sdlFont == nullptr)
    {
        LogCat::e(std::source_location::current(), "Failed to load font: ", actualPath.value());
        return false;
    }
    resourcePackManager->SetFont(sdlFont);
    return true;
}

bool glimmer::App::InitAudio()
{
    Config* config = appContext_->GetConfig();
    SDL_AudioSpec audioSpec;
    const std::string& audioFormat = config->audio.format;
    if (audioFormat == "U8")
    {
        audioSpec.format = SDL_AUDIO_U8;
    }
    else if (audioFormat == "S16")
    {
        audioSpec.format = SDL_AUDIO_S16;
    }
    else if (audioFormat == "S32")
    {
        audioSpec.format = SDL_AUDIO_S32;
    }
    else
    {
        audioSpec.format = SDL_AUDIO_F32;
    }

    audioSpec.channels = config->audio.channels;
    audioSpec.freq = config->audio.freq;

    mixer_ = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec);
    if (mixer_ == nullptr)
    {
        return false;
    }

    ResourcePackManager* resourcePackManager = appContext_->GetResourcePackManager();
    resourcePackManager->SetMixer(mixer_);
    appContext_->LoadMainMenuBGM();
    const AudioContext* audioContext = appContext_->GetAudioContext();
    if (audioContext == nullptr)
    {
        return false;
    }
    AudioManager* audioManager = audioContext->GetAudioManager();
    if (audioManager == nullptr)
    {
        return false;
    }
    audioManager->SetMixer(mixer_);

    for (const AudioTrack& trackConfig : config->audio.track)
    {
        audioManager->CreateTracks(trackConfig.type, trackConfig.trackCount);
        audioManager->SetTypeVolume(trackConfig.type, trackConfig.volume);
    }
    audioManager->SetMasterVolume(config->audio.masterVolume);

    AppContext::RestoreColorRenderer(renderer_);
    return true;
}

bool glimmer::App::CheckWindowSizeChange(WindowContext* windowContext, const int& windowWidth,
                                         const int& windowHeight)
{
    bool changed = false;

    if (windowHeight != windowContext->GetWindowHeight())
    {
        changed = true;
        windowContext->SetWindowHeight(windowHeight);
    }
    if (windowWidth != windowContext->GetWindowWidth())
    {
        changed = true;
        windowContext->SetWindowWidth(windowWidth);
    }
    return changed;
}

glimmer::App::~App()
{
    if (initSDLMixSuccess_)
    {
        MIX_Quit();
    }
    if (window != nullptr)
    {
        SDL_DestroyWindow(window);
    }
    if (initSDLTtfSuccess_)
    {
        TTF_Quit();
    }
    if (initSDLSuccess_)
    {
        SDL_Quit();
    }
}

glimmer::App::App(AppContext* appContext) :
    appContext_(appContext)
{
}

bool glimmer::App::Init()
{
    return InitSDL() &&
        InitWindowAndRenderer() &&
        InitFont() &&
        InitAudio();
}

void glimmer::App::Run() const
{
    const auto sceneManager = appContext_->GetSceneManager();
    const auto config = appContext_->GetConfig();

    InitScenesAndConsole();

    Uint64 frameStart = SDL_GetTicks();
    Uint64 lastInputTime = SDL_GetTicks();
    float deltaTime = 0.0F;
    //+1 is used to ensure that the first frame receives the broadcast.
    //+1是为了让第一帧收到广播。
    uint64_t configFingerprint = config->GetFingerprint() + 1;

    AppEventLoop eventLoop(appContext_, lastInputTime);
    AppRenderer renderer(appContext_, renderer_);

    WindowContext* windowContext = appContext_->GetWindowContext();
    if (windowContext == nullptr)
    {
        return;
    }
    MainThreadDispatcher* mainThreadDispatcher = appContext_->GetMainThreadDispatcher();
    if (mainThreadDispatcher == nullptr)
    {
        return;
    }
    RmlContext* rmlContext = appContext_->GetRmlContext();
    if (rmlContext == nullptr)
    {
        return;
    }
    // rmlContext->LoadDocument("/home/coldmint/projects/mods/resource_packs/glimmerworks_res/layouts/@core/splash/splash.rml");
    while (windowContext->IsRunning() && sceneManager->GetSceneCount() > 0)
    {
        int windowWidth = 0;
        int windowHeight = 0;
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);

        if (CheckWindowSizeChange(windowContext, windowWidth, windowHeight))
        {
            HandleWindowSizeChange(windowWidth, windowHeight);
        }

        CheckConfigChange(configFingerprint);

        const float targetFrameTime = CalculateTargetFrameTime(frameStart, lastInputTime);
        const auto targetFrameTimeMs = static_cast<Uint32>(targetFrameTime * 1000.0F);

        NotifyFrameStart();
        mainThreadDispatcher->ProcessMainThreadTasks();
        eventLoop.ProcessEvents(frameStart);
        rmlContext->UpdateContext();
        UpdateScenes(deltaTime);
        rmlContext->RenderContext();
        renderer.RenderFrame(windowWidth, windowHeight, frameStart, deltaTime);
        const Uint64 frameTimeMs = SDL_GetTicks() - frameStart;
        if (frameTimeMs < targetFrameTimeMs)
        {
            SDL_Delay(targetFrameTimeMs - frameTimeMs);
        }
        const Uint64 actualFrameEnd = SDL_GetTicks();
        deltaTime = static_cast<float>(actualFrameEnd - frameStart) / 1000.0F;
        frameStart = actualFrameEnd;
    }
}

void glimmer::App::HandleWindowSizeChange(const int& windowWidth, const int& windowHeight) const
{
    const auto sceneManager = appContext_->GetSceneManager();
    const auto& overlayScenes = sceneManager->GetOverlayScenes();
    for (const auto overlayScene : std::ranges::reverse_view(overlayScenes))
    {
        overlayScene->OnWindowSizeChanged(windowWidth, windowHeight);
    }
    if (Scene* topScene = sceneManager->GetTopScene(); topScene != nullptr)
    {
        topScene->OnWindowSizeChanged(windowWidth, windowHeight);
    }
}

float glimmer::App::CalculateTargetFrameTime(const uint64_t frameStart, const uint64_t lastInputTime) const
{
    const auto config = appContext_->GetConfig();
    const float idleDelay = config->window.idleDelay;

    if (idleDelay == -1)
    {
        return 1.0F / config->window.normalTargetFps;
    }

    if (static_cast<float>(frameStart - lastInputTime) * 0.001F < idleDelay)
    {
        return 1.0F / config->window.normalTargetFps;
    }
    return 1.0F / config->window.idleTargetFps;
}

bool glimmer::App::CheckConfigChange(uint64_t& configFingerprint) const
{
    const auto config = appContext_->GetConfig();
    const uint64_t nowConfigFingerprint = config->GetFingerprint();
    if (configFingerprint == nowConfigFingerprint)
    {
        return false;
    }

    if (CommandHookManager* commandHookManager = appContext_->GetConsoleContext()->GetCommandHookManager();
        commandHookManager != nullptr)
    {
        commandHookManager->LoadHookFromConfig(config->commandHooks);
    }

    const auto sceneManager = appContext_->GetSceneManager();
    const auto& overlayScenes = sceneManager->GetOverlayScenes();
    for (const auto overlayScene : std::ranges::reverse_view(overlayScenes))
    {
        overlayScene->OnConfigChanged(config);
    }
    if (Scene* topScene = sceneManager->GetTopScene(); topScene != nullptr)
    {
        topScene->OnConfigChanged(config);
    }

    configFingerprint = nowConfigFingerprint;
    return true;
}

void glimmer::App::NotifyFrameStart() const
{
    const auto sceneManager = appContext_->GetSceneManager();
    const auto& overlayScenes = sceneManager->GetOverlayScenes();
    for (const auto overlayScene : std::ranges::reverse_view(overlayScenes))
    {
        overlayScene->OnFrameStart();
    }
    if (Scene* topScene = sceneManager->GetTopScene(); topScene != nullptr)
    {
        topScene->OnFrameStart();
    }
}

void glimmer::App::UpdateScenes(const float deltaTime) const
{
    const auto sceneManager = appContext_->GetSceneManager();
    const auto& overlayScenes = sceneManager->GetOverlayScenes();
    for (const auto overlay : overlayScenes)
    {
        overlay->Update(deltaTime);
    }
    if (Scene* topScene = sceneManager->GetTopScene(); topScene != nullptr)
    {
        topScene->Update(deltaTime);
    }
}

void glimmer::App::InitScenesAndConsole() const
{
    auto sceneManager = appContext_->GetSceneManager();
    sceneManager->PushScene(std::make_unique<SplashScene>(appContext_));
#if  !defined(NDEBUG)
    sceneManager->AddOverlayScene(std::make_unique<DebugOverlay>(appContext_));
#endif

    ConsoleWorker* consoleWorker = appContext_->GetConsoleContext()->GetConsoleWorker();
    if (consoleWorker == nullptr)
    {
        return;
    }
    consoleWorker->PushOnMessage(
        std::make_unique<std::function<void(const std::string&)>>([this](const std::string& text)
        {
            if (appContext_ == nullptr)
            {
                return;
            }
            appContext_->AddUIMessage(text);
        })
    );
}
