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
#include <memory>
#include <vector>

#include <SDL3/SDL_gpu.h>

#include "core/context/AppContext.h"
#include "core/gpu/RenderQueue.h"

namespace glimmer {
    class ResourceLocator;
    class SceneManager;
    class RmlContext;
    class IPass;

    /**
     * AppRenderer
     * 应用渲染器
     *
     * Drives one frame of rendering: clears the per-frame RenderQueue, lets
     * the scenes/overlays submit their commands, then delegates the actual GPU
     * work to an ordered sequence of Pass objects (scene, clear, lighting, UI)
     * before submitting the frame.
     * 驱动一帧的渲染：清空每帧的 RenderQueue，让场景/覆盖层提交命令，然后把
     * 实际 GPU 工作委托给有序的 Pass 序列（场景、清屏、光照、UI），
     * 最后提交这一帧。
     */
    class AppRenderer {
        AppContext *appContext_ = nullptr;
        RenderQueue renderQueue_;
        SDL_GPUDevice *device_ = nullptr;
        SDL_Window *window_ = nullptr;
        ResourceLocator *resourceLocator_ = nullptr;
        SceneManager *sceneManager_ = nullptr;
        std::vector<std::unique_ptr<IPass> > passes_;

        void RenderOverlays();

    public:
        explicit AppRenderer(AppContext *appContext);

        ~AppRenderer();

        /**
         * RenderFrame
         * 渲染一帧。
         * @param windowWidth
         * @param windowHeight
         */
        void RenderFrame(int windowWidth, int windowHeight);
    };
}
