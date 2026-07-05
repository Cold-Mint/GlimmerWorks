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
#pragma once
#include "Scene.h"
#include "core/world/WorldContext.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>

namespace glimmer {
    class WorldScene : public Scene {
        std::unique_ptr<WorldContext> worldContext_;

    public:
        explicit WorldScene(AppContext *context, std::unique_ptr<WorldContext> worldContext);

        void OnFrameStart() override;

        bool HandleEvent(const SDL_Event &event) override;

        bool OnBackPressed() override;

        void OnWindowClose() override;

        void Update(float delta) override;

        void OnWindowSizeChanged(const int& width, const int& height) override;

        void OnConfigChanged(const Config* config) override;

        void Render(SDL_Renderer *renderer) override;

        void RenderImGui(SDL_Renderer* renderer) override;
    };
}
