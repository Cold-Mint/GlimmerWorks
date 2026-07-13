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
#include "scene/SceneManager.h"
#include "console/ConsoleWorker.h"
#include "console/CommandManager.h"
#include "CommandHookManager.h"
#include <ranges>

#include "RmlUi/Core/Context.h"


glimmer::AppEventLoop::AppEventLoop(AppContext* appContext, Uint64& lastInputTime) :
    appContext_(appContext), rmlContext_(appContext->GetRmlContext()->GetRmlContext()),
    lastInputTime_(lastInputTime)
{
}

int glimmer::AppEventLoop::SdlModToRmlModifier(const SDL_Keymod sdl_mod)
{
    int mask = 0;
    if (sdl_mod & SDL_KMOD_CTRL)
    {
        mask |= Rml::Input::KM_CTRL;
    }
    if (sdl_mod & SDL_KMOD_SHIFT)
    {
        mask |= Rml::Input::KM_SHIFT;
    }
    if (sdl_mod & SDL_KMOD_ALT)
    {
        mask |= Rml::Input::KM_ALT;
    }
    if (sdl_mod & SDL_KMOD_GUI)
    {
        mask |= Rml::Input::KM_META;
    }
    if (sdl_mod & SDL_KMOD_CAPS)
    {
        mask |= Rml::Input::KM_CAPSLOCK;
    }
    if (sdl_mod & SDL_KMOD_NUM)
    {
        mask |= Rml::Input::KM_NUMLOCK;
    }
    return mask;
}

void glimmer::AppEventLoop::SendEventToRML(const SDL_Event& event) const
{
    if (rmlContext_ == nullptr)
    {
        return;
    }

    int modState = SdlModToRmlModifier(SDL_GetModState());
    switch (event.type)
    {
    case SDL_EVENT_MOUSE_MOTION:
        rmlContext_->ProcessMouseMove(
            static_cast<int>(event.motion.x),
            static_cast<int>(event.motion.y),
            modState
        );
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        //event.button.button - 1 Here, -1 is because the index of the left mouse button in SDL is 1. And in RML, the condition index == 0 is used to determine a click event.
        //event.button.button - 1 其中-1是因为SDL的按钮左键index为1。而rml内判断index==0为click事件。
        rmlContext_->ProcessMouseButtonDown(
            event.button.button - 1,
            modState
        );
        break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
        rmlContext_->ProcessMouseButtonUp(
            event.button.button - 1,
            modState
        );
        break;
    case SDL_EVENT_MOUSE_WHEEL:
        {
            Rml::Vector2f delta{
                (event.wheel.x),
                (event.wheel.y)
            };
            rmlContext_->ProcessMouseWheel(delta, modState);
            break;
        }
    case SDL_EVENT_KEY_DOWN:
        rmlContext_->ProcessKeyDown(
            static_cast<Rml::Input::KeyIdentifier>(event.key.scancode),
            modState
        );
        break;

    case SDL_EVENT_KEY_UP:
        rmlContext_->ProcessKeyUp(
            static_cast<Rml::Input::KeyIdentifier>(event.key.scancode),
            modState
        );
        break;
    case SDL_EVENT_TEXT_INPUT:
        rmlContext_->ProcessTextInput(event.text.text);
        break;
    case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        rmlContext_->ProcessMouseLeave();
        break;
    default:
        break;
    }
}

void glimmer::AppEventLoop::ProcessEvents(const uint64_t frameStart) const
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
        if (DispatchEventToScene(event))
        {
            continue;
        }
        SendEventToRML(event);
    }
}

bool glimmer::AppEventLoop::HandleSystemEvent(const SDL_Event& event) const
{
    auto sceneManager = appContext_->GetSceneManager();
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

void glimmer::AppEventLoop::HandleCommandHooks(const SDL_Event& event) const
{
    auto commandHookManager = appContext_->GetConsoleContext()->GetCommandHookManager();
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

    auto consoleWorker = appContext_->GetConsoleContext()->GetConsoleWorker();
    auto commandManager = appContext_->GetConsoleContext()->GetCommandManager();
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

bool glimmer::AppEventLoop::DispatchEventToScene(const SDL_Event& event) const
{
    const auto sceneManager = appContext_->GetSceneManager();
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
    if (handled)
    {
        return true;
    }
    if (Scene* topScene = sceneManager->GetTopScene(); topScene != nullptr)
    {
        return topScene->HandleEvent(event);
    }
    return false;
}
