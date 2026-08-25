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
#include "core/context/AppContext.h"
#include "core/gpu/GpuRenderer.h"
#include "core/gpu/RenderQueue.h"

namespace glimmer {
    /**
     * AppRenderer
     * 应用渲染器
     *
     * Drives one frame of rendering: clears the per-frame RenderQueue, lets
     * the scenes/overlays/UI messages submit their commands, then has the
     * GpuRenderer flush the sorted queue into the game layer, renders RmlUi
     * into the ui layer and composites everything to the swapchain.
     * 驱动一帧的渲染：清空每帧的 RenderQueue，让场景/覆盖层/UI 消息提交
     * 命令，然后由 GpuRenderer 把排好序的队列冲刷进 game 层，将 RmlUi
     * 渲染进 ui 层，最后把全部内容合成到交换链。
     */
    class AppRenderer {
        AppContext *appContext_ = nullptr;
        GpuRenderer *renderer_ = nullptr;
        RenderQueue renderQueue_;

        void RenderUiMessage(int windowHeight, uint64_t frameStart);

        void RenderScenes();

        void RenderOverlays();

    public:
        AppRenderer(AppContext *appContext, GpuRenderer *renderer);

        void RenderFrame(const RmlContext *rmlContext, int windowWidth, int windowHeight, uint64_t frameStart,
                         float deltaTime);
    };
}
