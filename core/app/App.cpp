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
#include "core/scene/SceneManager.h"
#include "core/console/ConsoleWorker.h"
#include "core/console/hook/CommandHookManager.h"
#include "RmlUi/Core/Context.h"
#include "core/scene/ConsoleOverlay.h"
#include "core/scene/UIMessageOverlay.h"
#include "core/scene/DialogOverlay.h"
#include "core/tick/TickWorker.h"


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
    if (tickWorker_ != nullptr) {
        tickWorker_->RemoveCallback(this);
    }
}

glimmer::App::App(AppContext *appContext) : appContext_(appContext) {
    tickWorker_ = appContext_->GetTickWorker();
    tickWorker_->AddCallback(this);
    sceneManager_ = appContext_->GetSceneManager();
}

void glimmer::App::Run() const {
    LogCat::i(LogLabel::DEFAULT, "starting_app_main_loop", "Starting application main loop");
    const auto config = appContext_->GetConfig();

    LogCat::i(LogLabel::DEFAULT, "initializing_scenes_and_console", "Initializing scenes and console");
    InitScenesAndConsole();

    uint64_t frameStart = SDL_GetTicks();
    uint64_t lastActiveTime = frameStart;
    float deltaTime = 0.0F;
    uint32_t configVersion = config->GetVersion();

    LogCat::i(LogLabel::DEFAULT, "creating_event_loop_and_renderer", "Creating event loop and renderer");
    AppEventLoop eventLoop(appContext_);
    AppRenderer renderer(appContext_);

    WindowContext *windowContext = appContext_->GetWindowContext();
    if (windowContext == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "window_context_is_null",
                  "windowContext is nullptr");
        return;
    }
    MainThreadDispatcher *mainThreadDispatcher = appContext_->GetMainThreadDispatcher();
    if (mainThreadDispatcher == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "main_thread_dispatcher_is_null",
                  "mainThreadDispatcher is nullptr");
        return;
    }
    RmlContext *rmlContext = appContext_->GetRmlContext();
    if (rmlContext == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "rml_context_is_null", "rmlContext is nullptr");
        return;
    }
    Rml::Context *rmlContextCore = rmlContext->GetRmlContext();
    if (rmlContextCore != nullptr) {
        rmlContextCore->SetDensityIndependentPixelRatio(config->Get<float>(uiScale_));
    }
    LogCat::i(LogLabel::DEFAULT, "entering_main_game_loop", "Entering main game loop");
    int windowWidth = 0;
    int windowHeight = 0;
    SDL_Window *window = windowContext->GetWindow();
    if (window == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "window_is_null", "window is nullptr");
        return;
    }
    bool wasIdle = false;
    while (sceneManager_->GetSceneCount() > 0) {
        LogCat::IncrementFrameCount();
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);
        if (CheckWindowSizeChange(windowContext, windowWidth, windowHeight)) {
            LogCat::i(LogLabel::DEFAULT, "window_size_changed", "Window size changed: {}x{}", windowWidth,
                      windowHeight);
            if (rmlContextCore != nullptr) {
                rmlContextCore->SetDimensions({windowWidth, windowHeight});
            }
            HandleWindowSizeChange(windowWidth, windowHeight);
        }
        if (CheckConfigChange(config, configVersion)) {
            LogCat::i(LogLabel::DEFAULT, "configuration_changed", "Configuration changed, reloading hooks and scenes");
        }
        if (eventLoop.GetPressedKeyCount() > 0) {
            lastActiveTime = frameStart;
        }
        const auto normalTargetFps = config->Get<float>(normalTargetFps_);
        const float targetFrameTime = CalculateTargetFrameTime(normalTargetFps, frameStart, lastActiveTime);
        const bool isIdle = targetFrameTime > 1.0F / normalTargetFps;
        if (isIdle != wasIdle) {
            if (isIdle) {
                LogCat::d(LogLabel::FRAME_RATE, "idle_state_entered",
                          "Idle state entered, target frame time {:.4f}s", targetFrameTime);
            } else {
                LogCat::d(LogLabel::FRAME_RATE, "idle_state_exited",
                          "Idle state exited, target frame time {:.4f}s", targetFrameTime);
            }
            wasIdle = isIdle;
        }
        const auto targetFrameTimeMs = static_cast<uint32_t>(targetFrameTime * 1000.0F);
        NotifyFrameStart();
        mainThreadDispatcher->ProcessMainThreadTasks();
        eventLoop.ProcessEvents();
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
    LogCat::i(LogLabel::DEFAULT, "main_game_loop_exited", "Main game loop exited");
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

float glimmer::App::
CalculateTargetFrameTime(const float normalTargetFps, const uint64_t frameStart, const uint64_t lastActiveTime) const {
    const auto config = appContext_->GetConfig();
    const auto idleDelay = config->Get<float>(idleDelay_);
    if (idleDelay == -1) {
        return 1.0F / normalTargetFps;
    }
    if (static_cast<float>(frameStart - lastActiveTime) * 0.001F < idleDelay) {
        return 1.0F / normalTargetFps;
    }
    return 1.0F / config->Get<float>(idleTargetFps_);
}

bool glimmer::App::CheckConfigChange(const Config *config, uint32_t &outConfigVersion) const {
    const uint32_t nowConfigVersion = config->GetVersion();
    if (outConfigVersion == nowConfigVersion) {
        return false;
    }
    if (const RmlContext *rmlContext = appContext_->GetRmlContext(); rmlContext != nullptr) {
        if (Rml::Context *rmlContextCore = rmlContext->GetRmlContext(); rmlContextCore != nullptr) {
            rmlContextCore->SetDensityIndependentPixelRatio(config->Get<float>(uiScale_));
        }
    }

    if (CommandHookManager *commandHookManager = appContext_->GetConsoleContext()->GetCommandHookManager();
        commandHookManager != nullptr) {
        commandHookManager->LoadHookFromConfig(config->CommandHooks());
    }

    const auto &overlayScenes = sceneManager_->GetOverlayScenes();
    for (const auto overlayScene: std::ranges::reverse_view(overlayScenes)) {
        overlayScene->OnConfigChanged(config);
    }
    if (Scene *topScene = sceneManager_->GetTopScene(); topScene != nullptr) {
        topScene->OnConfigChanged(config);
    }
    outConfigVersion = nowConfigVersion;
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
#if  !defined(NDEBUG)
    sceneManager->AddOverlayScene(std::make_unique<DebugOverlay>(appContext_));
#endif
    sceneManager->AddOverlayScene(std::make_unique<ConsoleOverlay>(appContext_));
    sceneManager->AddOverlayScene(std::make_unique<UIMessageOverlay>(appContext_));
    std::unique_ptr<DialogOverlay> dialogOverlay = std::make_unique<DialogOverlay>(appContext_);
    appContext_->SetDialogOverlay(dialogOverlay.get());
    sceneManager->AddOverlayScene(std::move(dialogOverlay));
    ConsoleWorker *consoleWorker = appContext_->GetConsoleContext()->GetConsoleWorker();
    if (consoleWorker == nullptr) {
        return;
    }
    consoleWorker->PushOnMessage(
        std::make_unique<std::function<void(const std::string &)> >([this](const std::string &text) {
            if (appContext_ == nullptr) {
                return;
            }
            appContext_->AddUIMessage(text);
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
