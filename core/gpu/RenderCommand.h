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

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_stdinc.h>

#include "RenderLayer.h"
#include "SpriteVertex.h"
#include "core/mod/resourcePack/GPUSamplerResourceResult.h"
#include "core/mod/resourcePack/TextureResourceResult.h"

namespace glimmer {
    class CompiledUniformBlock;

    /**
     * Flip flags for RenderQueue::DrawTextureRotated (replaces SDL_FlipMode).
     * RenderQueue::DrawTextureRotated 使用的翻转标志（替代 SDL_FlipMode）。
     */
    enum FlipFlags : Uint8 {
        FLIP_NONE = 0,
        FLIP_HORIZONTAL = 1,
        FLIP_VERTICAL = 2
    };

    /**
     * RenderCommand
     * 渲染命令
     *
     * One textured (or plain colored) quad submitted to the RenderQueue.
     * The four corners are stored in top-left, top-right, bottom-left,
     * bottom-right order and are expanded into two triangles (6 vertices)
     * when the queue is flushed by GpuRenderer.
     * 提交到 RenderQueue 的一个带纹理（或纯色）四边形。四个角点按左上、
     * 右上、左下、右下顺序存储，GpuRenderer 冲刷队列时展开为两个三角形
     * （6 个顶点）。
     *
     * A command whose texture is nullptr renders plain colored geometry
     * using the renderer's built-in white texture (rects/lines/points).
     * texture 为 nullptr 的命令使用渲染器内置的白色纹理绘制纯色几何图形
     * （矩形/线/点）。
     */
    struct RenderCommand {
        const TextureResourceResult *texture = nullptr;
        SDL_GPUSampler *sampler = nullptr;
        SpriteVertex corners[4] = {};
        RenderLayer layer = RenderLayer::Background;
        float depth = 0.0F;
        SDL_GPUGraphicsPipeline *pipeline = nullptr;
        //Optional uniform block to fill and push before drawing this command.
        //绘制此命令前要填充并推送的可选 uniform 块。
        const CompiledUniformBlock *uniformBlock = nullptr;
    };
}
