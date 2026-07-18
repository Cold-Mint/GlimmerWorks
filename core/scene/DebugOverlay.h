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
#if  !defined(NDEBUG)
#include "Scene.h"
#include "core/mod/resourcePack/ResourcePackManager.h"

namespace glimmer
{
    struct LangsResources;
    /**
     * Debug the overlay layer
     * 调试叠加层
     * It can be displayed on any scene. Display frps, screen coordinates and other information.
     * 可以显示在任意场景上。显示frps，屏幕坐标等信息。
     */
    class DebugOverlay : public Scene
    {
        float fps_ = 0.0F;
        float frameTimeMs_ = 0.0F;
        float fpsAccumTime_ = 0.0F;
        int fpsFrameCount_ = 0;
        bool displayDebugPanel_ = false;
        int windowWidth_ = 0;
        int windowHeight_ = 0;
        float uiScale_ = 0.0F;
        std::unordered_map<int, std::shared_ptr<SDL_Texture>> numberTextureMap_;
        std::unordered_map<uint64_t, std::shared_ptr<SDL_Texture>> fpsTextures_;
        ResourcePackManager* resourcePackManager_ = nullptr;
        PreloadColors* preloadColors_ = nullptr;
        LangsResources* langsResources_ = nullptr;

    public:
        explicit DebugOverlay(AppContext* context);

        void Update(float delta) override;

        void Render(SDL_Renderer* renderer) override;

        void OnConfigChanged(const Config* config) override;

        void OnWindowSizeChanged(const int& width, const int& height) override;

        ~DebugOverlay() override = default;
    };
}

#endif
