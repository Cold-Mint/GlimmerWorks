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
#include "AppEventLoop.h"
#include "backends/imgui_impl_sdl3.h"
#include "scene/SceneManager.h"
#include "console/ConsoleWorker.h"
#include "console/CommandManager.h"
#include "CommandHookManager.h"
#include <ranges>

namespace glimmer
{
    AppEventLoop::AppEventLoop(AppContext* appContext, Uint64& lastInputTime) :
        appContext_(appContext),
        lastInputTime_(lastInputTime)
    {
    }

    void AppEventLoop::ProcessEvents(const uint64_t frameStart) const
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            lastInputTime_ = frameStart;
            if (HandleSystemEvent(event))
            {
                continue;
            }
            HandleCommandHooks(event);
            DispatchEvent(event);
        }
    }

    bool AppEventLoop::HandleSystemEvent(const SDL_Event& event) const
    {
        auto* sceneManager = appContext_->GetSceneManager();
#ifdef __ANDROID__
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_AC_BACK)
        {
            if (Scene* topScene = sceneManager->GetTopScene(); topScene != nullptr)
            {
                if (!topScene->OnBackPressed())
                {
                    sceneManager->PopScene();
                }
            }
            return true;
        }
#else
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE && !event.key.repeat)
        {
            bool handled = false;
            const auto& overlayScenes = sceneManager->GetOverlayScenes();
            for (const auto overlayScene : std::ranges::reverse_view(overlayScenes))
            {
                if (overlayScene->OnBackPressed())
                {
                    handled = true;
                    break;
                }
            }
            Scene* topScene = sceneManager->GetTopScene();
            if (!handled && topScene != nullptr && !topScene->OnBackPressed())
            {
                sceneManager->PopScene();
            }
            return true;
        }
#endif
        if (event.type == SDL_EVENT_QUIT)
        {
            const auto& overlayScenes = sceneManager->GetOverlayScenes();
            for (const auto overlayScene : std::ranges::reverse_view(overlayScenes))
            {
                overlayScene->OnWindowClose();
            }
            if (Scene* topScene = sceneManager->GetTopScene(); topScene != nullptr)
            {
                topScene->OnWindowClose();
            }
            appContext_->ExitApp();
            return true;
        }
        return false;
    }

    void AppEventLoop::HandleCommandHooks(const SDL_Event& event) const
    {
        auto* commandHookManager = appContext_->GetCommandHookManager();
        if (commandHookManager == nullptr)
        {
            return;
        }
        uint16_t code = 0;
        const auto type = static_cast<SDL_EventType>(event.type);
        bool isKey = false;
        bool useMouse = false;

        if (type == SDL_EVENT_KEY_DOWN || type == SDL_EVENT_KEY_UP)
        {
            code = event.key.scancode;
            isKey = true;
        }
        else if (type == SDL_EVENT_MOUSE_BUTTON_DOWN || type == SDL_EVENT_MOUSE_BUTTON_UP)
        {
            code = event.button.button;
            useMouse = true;
        }
        else
        {
            return;
        }

        const std::vector<CommandHookEntry*>& commandHookEntry =
            commandHookManager->GetCommandHookVector(CommandHookEntry::GetKey(type, code));
        if (commandHookEntry.empty())
        {
            return;
        }

        auto* consoleWorker = appContext_->GetConsoleWorker();
        auto* commandManager = appContext_->GetCommandManager();
        for (const auto& commandHook : commandHookEntry)
        {
            if (isKey && commandHook->keyRepeat != event.key.repeat)
            {
                continue;
            }
            if (useMouse)
            {
                consoleWorker->CreateRequest(commandHook->command,
                                             commandManager->GetMouseCommandSender());
            }
            else
            {
                consoleWorker->CreateRequest(commandHook->command,
                                             commandManager->GetDefaultCommandSender());
            }
        }
    }

    void AppEventLoop::DispatchEvent(const SDL_Event& event) const
    {
        auto* sceneManager = appContext_->GetSceneManager();
        const auto& overlayScenes = sceneManager->GetOverlayScenes();

        bool handled = false;
        for (const auto overlayScene : std::ranges::reverse_view(overlayScenes))
        {
            if (overlayScene->HandleEvent(event))
            {
                handled = true;
                break;
            }
        }
        if (!handled)
        {
            if (Scene* topScene = sceneManager->GetTopScene(); topScene != nullptr)
            {
                if (topScene->HandleEvent(event))
                {
                    handled = true;
                }
            }
        }
        if (!handled)
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
        }
    }
}
