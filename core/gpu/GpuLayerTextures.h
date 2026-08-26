/*
 * Copyright (C) 2025-2026  Cold-Mint <cold_mint@qq.com>
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
 * 版权(C) 2025-2026  Cold-Mint <cold_mint@qq.com>
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

#include "GpuTexture.h"

namespace glimmer {
    class GpuContext;

    /**
     * GpuLayerTextures
     * GPU 分层渲染纹理
     *
     * Owns the offscreen layer textures used by the layered renderer:
     * game, ui and composite layers. Recreates them when the swapchain size
     * changes.
     * 持有分层渲染器使用的离屏层纹理：game、ui 与 composite 层。当交换链
     * 尺寸变化时重新创建它们。
     */
    class GpuLayerTextures {
        std::unique_ptr<GpuTexture> gameLayerTexture_;
        std::unique_ptr<GpuTexture> uiLayerTexture_;
        std::unique_ptr<GpuTexture> compositeLayerTexture_;

    public:
        GpuLayerTextures() = default;

        /**
         * Create or recreate the layer textures to match the given size.
         * 创建或重建层纹理以匹配给定尺寸。
         * @param context context GPU 上下文
         * @param width width 目标宽度
         * @param height height 目标高度
         * @return true if all layer textures are valid.
         * 所有层纹理有效时返回 true。
         */
        bool EnsureLayers(GpuContext *context, Uint32 width, Uint32 height);

        /**
         * Release the layer textures.
         * 释放层纹理。
         */
        void Shutdown();

        [[nodiscard]] GpuTexture *GetGameLayer() const { return gameLayerTexture_.get(); }

        [[nodiscard]] GpuTexture *GetUiLayer() const { return uiLayerTexture_.get(); }

        [[nodiscard]] GpuTexture *GetCompositeLayer() const { return compositeLayerTexture_.get(); }
    };
}
