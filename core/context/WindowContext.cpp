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
#include "WindowContext.h"

#include <SDL3/SDL.h>

bool glimmer::WindowContext::StopInput() const
{
    if (window_ == nullptr)
    {
        return false;
    }
    return SDL_StopTextInput(window_);
}

bool glimmer::WindowContext::StartInput() const
{
    if (window_ == nullptr)
    {
        return false;
    }
    return SDL_StartTextInput(window_);
}

void glimmer::WindowContext::SetWindow(SDL_Window* window)
{
    window_ = window;
}

void glimmer::WindowContext::SetRenderer(SDL_Renderer* renderer)
{
    renderer_ = renderer;
}

void glimmer::WindowContext::SetWindowWidth(int width)
{
    windowWidth_ = width;
}

void glimmer::WindowContext::SetWindowHeight(int height)
{
    windowHeight_ = height;
}

void glimmer::WindowContext::SetWindowTitle(const char* title) const
{
    SDL_SetWindowTitle(window_, title);
}

SDL_Renderer* glimmer::WindowContext::GetRenderer() const
{
    return renderer_;
}

int glimmer::WindowContext::GetWindowWidth() const
{
    return windowWidth_;
}

int glimmer::WindowContext::GetWindowHeight() const
{
    return windowHeight_;
}

bool glimmer::WindowContext::IsRunning() const
{
    return isRunning_;
}

void glimmer::WindowContext::Exit()
{
    isRunning_ = false;
}
