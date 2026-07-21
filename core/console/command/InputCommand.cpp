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
#include "InputCommand.h"

#include <string>

#include "fmt/xchar.h"
#include "core/LangsResources.h"
#include "core/context/AppContext.h"
#include "core/Constants.h"

#include <SDL3/SDL.h>

namespace glimmer
{

    InputCommand::InputCommand(AppContext* appContext) : Command(appContext)
    {
    }

    const std::string& InputCommand::GetName() const
    {
        return INPUT_COMMAND_NAME;
    }

    void InputCommand::PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
    {
        if (commandArgs == nullptr || strings == nullptr)
        {
            return;
        }
        strings->emplace_back("[key|mouse|text]");
        strings->emplace_back("[down|up|click|move|scroll]");
        strings->emplace_back("[param1:string|number]");
        strings->emplace_back("[param2:string|number]");
        strings->emplace_back("[x:number]");
        strings->emplace_back("[y:number]");
    }

    bool InputCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                               const std::function<void(const std::string& text)>* onMessage)
    {
        const AppContext* appContext = GetAppContext();
        if (appContext == nullptr || commandArgs == nullptr || onMessage == nullptr)
        {
            return false;
        }
        const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
        const LangsResources* langsResources = appContext->GetLangsResources();
        if (langsResources == nullptr)
        {
            return false;
        }

        const int size = commandArgs->GetSize();
        if (size < 3)
        {
            onMessageRef(fmt::format(
                fmt::runtime(langsResources->insufficientParameterLength),
                3, size));
            return false;
        }

        std::string device = commandArgs->AsString(1);
        std::string action = commandArgs->AsString(2);

        if (device == "key")
        {
            if (size < 4)
            {
                onMessageRef(fmt::format(
                    fmt::runtime(langsResources->insufficientParameterLength),
                    4, size));
                return false;
            }
            std::string keyName = commandArgs->AsString(3);
            
            SDL_Scancode scancode = SDL_GetScancodeFromName(keyName.c_str());
            if (scancode == SDL_SCANCODE_UNKNOWN)
            {
                onMessageRef("Unknown key name: " + keyName);
                return false;
            }

            SDL_Event event{};
            if (action == "down")
            {
                event.type = SDL_EVENT_KEY_DOWN;
                event.key.scancode = scancode;
                event.key.key = SDL_GetKeyFromScancode(scancode, SDL_GetModState(), false);
                event.key.repeat = false;
                SDL_PushEvent(&event);
                onMessageRef("Simulated key down: " + keyName);
            }
            else if (action == "up")
            {
                event.type = SDL_EVENT_KEY_UP;
                event.key.scancode = scancode;
                event.key.key = SDL_GetKeyFromScancode(scancode, SDL_GetModState(), false);
                event.key.repeat = false;
                SDL_PushEvent(&event);
                onMessageRef("Simulated key up: " + keyName);
            }
            else
            {
                onMessageRef("Unknown key action: " + action);
                return false;
            }
            return true;
        }

        if (device == "mouse")
        {
            SDL_Event event{};
            if (action == "click")
            {
                if (size < 4)
                {
                    onMessageRef(fmt::format(
                        fmt::runtime(langsResources->insufficientParameterLength),
                        4, size));
                    return false;
                }
                std::string buttonStr = commandArgs->AsString(3);
                int button = SDL_BUTTON_LEFT;
                if (buttonStr == "right")
                {
                    button = SDL_BUTTON_RIGHT;
                }
                else if (buttonStr == "middle")
                {
                    button = SDL_BUTTON_MIDDLE;
                }

                int x = 0, y = 0;
                bool hasPosition = size >= 6;
                if (hasPosition)
                {
                    x = std::stoi(commandArgs->AsString(4));
                    y = std::stoi(commandArgs->AsString(5));
                    
                    event.type = SDL_EVENT_MOUSE_MOTION;
                    event.motion.x = static_cast<float>(x);
                    event.motion.y = static_cast<float>(y);
                    SDL_PushEvent(&event);
                }

                event.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
                event.button.button = button;
                if (hasPosition)
                {
                    event.button.x = static_cast<float>(x);
                    event.button.y = static_cast<float>(y);
                }
                SDL_PushEvent(&event);

                event.type = SDL_EVENT_MOUSE_BUTTON_UP;
                if (hasPosition)
                {
                    event.button.x = static_cast<float>(x);
                    event.button.y = static_cast<float>(y);
                }
                SDL_PushEvent(&event);

                if (hasPosition)
                {
                    onMessageRef("Simulated mouse click: " + buttonStr + " button at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
                }
                else
                {
                    onMessageRef("Simulated mouse click: " + buttonStr + " button");
                }
            }
            else if (action == "down")
            {
                if (size < 4)
                {
                    onMessageRef(fmt::format(
                        fmt::runtime(langsResources->insufficientParameterLength),
                        4, size));
                    return false;
                }
                std::string buttonStr = commandArgs->AsString(3);
                int button = SDL_BUTTON_LEFT;
                if (buttonStr == "right")
                {
                    button = SDL_BUTTON_RIGHT;
                }
                else if (buttonStr == "middle")
                {
                    button = SDL_BUTTON_MIDDLE;
                }

                event.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
                event.button.button = button;
                SDL_PushEvent(&event);
                onMessageRef("Simulated mouse down: " + buttonStr + " button");
            }
            else if (action == "up")
            {
                if (size < 4)
                {
                    onMessageRef(fmt::format(
                        fmt::runtime(langsResources->insufficientParameterLength),
                        4, size));
                    return false;
                }
                std::string buttonStr = commandArgs->AsString(3);
                int button = SDL_BUTTON_LEFT;
                if (buttonStr == "right")
                {
                    button = SDL_BUTTON_RIGHT;
                }
                else if (buttonStr == "middle")
                {
                    button = SDL_BUTTON_MIDDLE;
                }

                event.type = SDL_EVENT_MOUSE_BUTTON_UP;
                event.button.button = button;
                SDL_PushEvent(&event);
                onMessageRef("Simulated mouse up: " + buttonStr + " button");
            }
            else if (action == "move")
            {
                if (size < 5)
                {
                    onMessageRef(fmt::format(
                        fmt::runtime(langsResources->insufficientParameterLength),
                        5, size));
                    return false;
                }
                int x = std::stoi(commandArgs->AsString(3));
                int y = std::stoi(commandArgs->AsString(4));

                event.type = SDL_EVENT_MOUSE_MOTION;
                event.motion.x = static_cast<float>(x);
                event.motion.y = static_cast<float>(y);
                SDL_PushEvent(&event);
                onMessageRef("Simulated mouse move: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
            }
            else if (action == "scroll")
            {
                if (size < 4)
                {
                    onMessageRef(fmt::format(
                        fmt::runtime(langsResources->insufficientParameterLength),
                        4, size));
                    return false;
                }
                int scrollY = std::stoi(commandArgs->AsString(3));

                event.type = SDL_EVENT_MOUSE_WHEEL;
                event.wheel.x = 0;
                event.wheel.y = static_cast<float>(scrollY);
                SDL_PushEvent(&event);
                onMessageRef("Simulated mouse scroll: " + std::to_string(scrollY));
            }
            else
            {
                onMessageRef("Unknown mouse action: " + action);
                return false;
            }
            return true;
        }

        if (device == "text")
        {
            onMessageRef("Text input simulation is not supported in this version.");
            return false;
        }

        onMessageRef("Unknown input device: " + device);
        return false;
    }
}