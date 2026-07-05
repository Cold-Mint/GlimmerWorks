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
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "scene/SplashScene.h"
#include "scene/ConsoleOverlay.h"
#if  !defined(NDEBUG)
#include "scene/DebugOverlay.h"
#endif
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
    LogCat::i("Initializing SDL...");
#ifdef __ANDROID__
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
    SDL_SetHint("SDL_ANDROID_TRAP_BACK_BUTTON", "1");
#endif
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        LogCat::e("SDL_Init Error: ", SDL_GetError());
        return false;
    }
    initSDLSuccess_ = true;
    if (!MIX_Init())
    {
        LogCat::e("MIX_Init Error: ", SDL_GetError());
        return false;
    }
    initSDLMixSuccess_ = true;
    LogCat::i("Initializing SDL_ttf...");
    if (!TTF_Init())
    {
        LogCat::e("TTF_Init Error: ", SDL_GetError());
        return false;
    }
    initSDLTtfSuccess_ = true;
    LogCat::i("SDL initialized successfully.");
    return true;
}

bool glimmer::App::InitWindowAndRenderer()
{
    LogCat::i("Creating SDL window...");
    Config* config = appContext_->GetConfig();
    window = SDL_CreateWindow(
        "GlimmerWorks",
        config->window.width,
        config->window.height,
        config->window.fullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_RESIZABLE
    );
    if (window == nullptr)
    {
        LogCat::e("SDL_CreateWindow Error: ", SDL_GetError());
        return false;
    }
    LogCat::i("SDL window created successfully.");
    appContext_->SetWindow(window);

    LogCat::i("Creating SDL renderer...");
    renderer_ = SDL_CreateRenderer(window, nullptr);
    if (renderer_ == nullptr)
    {
        LogCat::e("SDL_CreateRenderer Error: ", SDL_GetError());
        return false;
    }
    SDL_SetRenderVSync(renderer_, config->window.vSync);
    appContext_->SetRenderer(renderer_);

    ResourcePackManager* resourcePackManager = appContext_->GetResourcePackManager();
    if (resourcePackManager == nullptr)
    {
        LogCat::e("ResourcePackManager is nullptr.");
        return false;
    }
    resourcePackManager->SetRenderer(renderer_, appContext_->GetPreloadColors());

    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
    SDL_SetDefaultTextureScaleMode(renderer_, SDL_SCALEMODE_PIXELART);
    LogCat::i("SDL renderer created successfully.");
    return true;
}

bool glimmer::App::InitImGui() const
{
    LogCat::i("Initializing ImGui context...");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    LogCat::i("ImGui context created.");

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    LogCat::i("Setting ImGui style...");
    auto* preloadColors = appContext_->GetPreloadColors();
    std::vector<std::pair<ImGuiCol, Color&>> colorMappings;
    colorMappings.emplace_back(ImGuiCol_Text, preloadColors->textColor);
    colorMappings.emplace_back(ImGuiCol_TextDisabled, preloadColors->textDisabledColor);
    colorMappings.emplace_back(ImGuiCol_TextLink, preloadColors->textLinkColor);
    colorMappings.emplace_back(ImGuiCol_TextSelectedBg, preloadColors->textSelectedBgColor);
    colorMappings.emplace_back(ImGuiCol_WindowBg, preloadColors->backgroundColor);
    colorMappings.emplace_back(ImGuiCol_Border, preloadColors->borderColor);
    colorMappings.emplace_back(ImGuiCol_BorderShadow, preloadColors->borderShadowColor);
    colorMappings.emplace_back(ImGuiCol_Separator, preloadColors->separatorColor);
    colorMappings.emplace_back(ImGuiCol_SeparatorHovered, preloadColors->separatorHoveredColor);
    colorMappings.emplace_back(ImGuiCol_SeparatorActive, preloadColors->separatorActiveColor);
    colorMappings.emplace_back(ImGuiCol_FrameBg, preloadColors->frameBgColor);
    colorMappings.emplace_back(ImGuiCol_FrameBgHovered, preloadColors->frameBgHoveredColor);
    colorMappings.emplace_back(ImGuiCol_FrameBgActive, preloadColors->frameBgActiveColor);
    colorMappings.emplace_back(ImGuiCol_Button, preloadColors->buttonColor);
    colorMappings.emplace_back(ImGuiCol_ButtonHovered, preloadColors->buttonHoveredColor);
    colorMappings.emplace_back(ImGuiCol_ButtonActive, preloadColors->buttonPressedColor);
    colorMappings.emplace_back(ImGuiCol_ScrollbarBg, preloadColors->scrollbarBgColor);
    colorMappings.emplace_back(ImGuiCol_ScrollbarGrab, preloadColors->scrollbarGrabColor);
    colorMappings.emplace_back(ImGuiCol_ScrollbarGrabHovered, preloadColors->scrollbarGrabHoveredColor);
    colorMappings.emplace_back(ImGuiCol_ScrollbarGrabActive, preloadColors->scrollbarGrabActiveColor);
    colorMappings.emplace_back(ImGuiCol_InputTextCursor, preloadColors->inputTextCursorColor);
    colorMappings.emplace_back(ImGuiCol_NavCursor, preloadColors->navCursorColor);
    colorMappings.emplace_back(ImGuiCol_Header, preloadColors->headerColor);
    colorMappings.emplace_back(ImGuiCol_HeaderHovered, preloadColors->headerHoveredColor);
    colorMappings.emplace_back(ImGuiCol_HeaderActive, preloadColors->headerActiveColor);

    for (const auto& [imguiColor,color] : colorMappings | std::views::as_const)
    {
        ImGui::GetStyle().Colors[imguiColor] = ColorUtils::ColorToImVec4(color);
    }

    LogCat::i("Initializing ImGui SDL3 backend for SDLRenderer...");
    if (!ImGui_ImplSDL3_InitForSDLRenderer(window, renderer_))
    {
        LogCat::e("ImGui_ImplSDL3_InitForSDLRenderer failed!");
        return false;
    }
    LogCat::i("ImGui SDL3 backend initialized successfully.");

    LogCat::i("Initializing ImGui SDLRenderer3 backend...");
    if (!ImGui_ImplSDLRenderer3_Init(renderer_))
    {
        LogCat::e("ImGui_ImplSDLRenderer3_Init failed!");
        return false;
    }
    LogCat::i("ImGui SDLRenderer3 backend initialized successfully.");
    return true;
}

bool glimmer::App::InitFont()
{
    Config* config = appContext_->GetConfig();
    ResourcePackManager* resourcePackManager = appContext_->GetResourcePackManager();

    const auto fontPathOpt = resourcePackManager->GetFontPath(
        config->mods.enabledResourcePack,
        appContext_->GetLanguage());

    if (!fontPathOpt.has_value())
    {
        LogCat::w("No font found for language '", appContext_->GetLanguage(), "', skipping font load");
        return true;
    }

    const std::string& fontPath = fontPathOpt.value();
    if (!appContext_->GetVirtualFileSystem()->Exists(fontPath))
    {
        return false;
    }

    auto actualPath = appContext_->GetVirtualFileSystem()->GetActualPath(fontPath);
    if (!actualPath.has_value())
    {
        LogCat::e("An error occurred when converting to the actual font path.");
        return false;
    }

    ImGuiIO& io = ImGui::GetIO();
    if (io.Fonts->AddFontFromFileTTF(actualPath.value().c_str(), 16.0F))
    {
        LogCat::d("Loaded font: ", fontPath);
    }
    else
    {
        LogCat::e("Failed to load font (ImGui error): ", fontPath);
    }
    TTF_Font* sdlFont = TTF_OpenFont(actualPath.value().c_str(), 16);
    if (sdlFont == nullptr)
    {
        LogCat::e("Failed to load SDL_ttf font: ", SDL_GetError());
    }
    else
    {
        LogCat::d("SDL_ttf font loaded: ", fontPath);
        resourcePackManager->SetFont(sdlFont);
    }

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
        LogCat::e("MIX_CreateMixerDevice failed! ", SDL_GetError());
        return false;
    }

    ResourcePackManager* resourcePackManager = appContext_->GetResourcePackManager();
    resourcePackManager->SetMixer(mixer_);
    appContext_->LoadMainMenuBGM();

    AudioManager* audioManager = appContext_->GetAudioManager();
    if (audioManager == nullptr)
    {
        LogCat::e("audioManager == null");
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

bool glimmer::App::CheckWindowSizeChange(const int& windowWidth, const int& windowHeight) const
{
    bool changed = false;
    if (windowHeight != appContext_->GetWindowHeight())
    {
        changed = true;
        appContext_->SetWindowHeight(windowHeight);
    }
    if (windowWidth != appContext_->GetWindowWidth())
    {
        changed = true;
        appContext_->SetWindowWidth(windowWidth);
    }
    return changed;
}

glimmer::App::~App()
{
    LogCat::i("Destroy the app");
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
        InitImGui() &&
        InitFont() &&
        InitAudio();
}

void glimmer::App::Run() const
{
    LogCat::i("Entering main loop...");
    const auto* sceneManager = appContext_->GetSceneManager();
    const auto* config = appContext_->GetConfig();

    InitScenesAndConsole();

    Uint64 frameStart = SDL_GetTicks();
    Uint64 lastInputTime = SDL_GetTicks();
    float deltaTime = 0.0F;
    //+1 is used to ensure that the first frame receives the broadcast.
    //+1是为了让第一帧收到广播。
    uint64_t configFingerprint = config->GetFingerprint() + 1;

    AppEventLoop eventLoop(appContext_, lastInputTime);
    AppRenderer renderer(appContext_, renderer_);

    while (appContext_->Running() && sceneManager->GetSceneCount() > 0)
    {
        int windowWidth = 0;
        int windowHeight = 0;
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);

        if (CheckWindowSizeChange(windowWidth, windowHeight))
        {
            HandleWindowSizeChange(windowWidth, windowHeight);
        }

        CheckConfigChange(configFingerprint);

        const float targetFrameTime = CalculateTargetFrameTime(frameStart, lastInputTime);
        const auto targetFrameTimeMs = static_cast<Uint32>(targetFrameTime * 1000.0F);

        NotifyFrameStart();
        appContext_->ProcessMainThreadTasks();

        eventLoop.ProcessEvents(frameStart);
        UpdateScenes(deltaTime);
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
    const auto* sceneManager = appContext_->GetSceneManager();
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
    const auto* config = appContext_->GetConfig();
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
    const auto* config = appContext_->GetConfig();
    const uint64_t nowConfigFingerprint = config->GetFingerprint();
    if (configFingerprint == nowConfigFingerprint)
    {
        return false;
    }

    if (CommandHookManager* commandHookManager = appContext_->GetCommandHookManager();
        commandHookManager != nullptr)
    {
        commandHookManager->LoadHookFromConfig(config->commandHooks);
    }

    const auto* sceneManager = appContext_->GetSceneManager();
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
    const auto* sceneManager = appContext_->GetSceneManager();
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
    const auto* sceneManager = appContext_->GetSceneManager();
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
    auto* sceneManager = appContext_->GetSceneManager();
    sceneManager->PushScene(std::make_unique<SplashScene>(appContext_));
    sceneManager->AddOverlayScene(std::make_unique<ConsoleOverlay>(appContext_));
#if  !defined(NDEBUG)
    sceneManager->AddOverlayScene(std::make_unique<DebugOverlay>(appContext_));
#endif

    ConsoleWorker* consoleWorker = appContext_->GetConsoleWorker();
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
