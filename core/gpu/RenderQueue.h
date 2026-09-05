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

#include <vector>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>

#include "RenderCommand.h"
#include "RenderLayer.h"
#include "core/mod/resourcePack/TextureResourceResult.h"

namespace glimmer {
    class RenderQueue {
        std::vector<RenderCommand> commands_;

        /**
         * Append one quad command to the queue.
         * 向队列追加一个四边形命令。
         * @param layer layer 命令所属渲染层
         * @param depth depth 层内深度（同层排序键，越小越先绘制）
         * @param texture texture 采样纹理（nullptr 表示纯色几何，使用渲染器内置白色纹理）
         * @param positions positions 4 个角点（左上、右上、左下、右下，像素坐标）
         * @param uvs uvs 4 个角点对应的纹理坐标
         * @param color color 顶点颜色（调色/透明度调制）
         * @param pipeline pipeline 渲染管线（nullptr 表示使用默认管线）
         * @param sampler sampler 采样器（nullptr 表示使用默认采样器）
         */
        void AppendQuad(RenderLayer layer, float depth, const TextureResourceResult *texture,
                        const SDL_FPoint positions[4], const SDL_FPoint uvs[4], const SDL_Color &color,
                        SDL_GPUGraphicsPipeline *pipeline = nullptr,
                        SDL_GPUSampler *sampler = nullptr);

    public:
        RenderQueue() = default;

        /**
         * Remove all queued commands (called once per frame before systems
         * submit new ones).
         * 移除所有已排队的命令（每帧在系统提交新命令前调用一次）。
         */
        void Clear();

        /**
         * Reserve storage for the expected number of commands to avoid
         * repeated reallocation while systems submit.
         * 为预期数量的命令预留存储，避免系统提交时反复重新分配。
         * @param commandCount commandCount 预计的命令数量
         */
        void Reserve(size_t commandCount);

        [[nodiscard]] size_t GetCommandCount() const;

        [[nodiscard]] bool IsEmpty() const;

        /**
         * Stable-sort the queued commands by (layer, depth) ascending.
         * Commands that compare equal keep their relative submission order,
         * which preserves the classic painter's algorithm inside a layer.
         * Called by AppRenderer::FlushScenePass; there is normally no need to
         * call it manually.
         * 按（层、depth）升序对队列中的命令进行稳定排序。比较结果相等的
         * 命令保持相对提交顺序，从而在层内保持经典的画家算法。
         * 由 AppRenderer::FlushScenePass 调用，通常无需手动调用。
         */
        void Sort();

        /**
         * @return All queued commands in their current (submission or
         * sorted) order. Used by GpuRenderer at flush time.
         * 当前顺序（提交顺序或排序后顺序）下的全部命令。供 GpuRenderer
         * 在冲刷时使用。
         */
        [[nodiscard]] const std::vector<RenderCommand> &GetCommands() const;

        /**
         * Queue a texture region drawn to a destination rectangle
         * (replaces SDL_RenderTexture).
         * 将一个纹理区域绘制到目标矩形的命令入队（替代 SDL_RenderTexture）。
         * @param layer layer 命令所属渲染层
         * @param depth depth 层内深度
         * @param texture texture 要绘制的纹理
         * @param src src 源矩形（像素，nullptr 表示整个纹理）
         * @param dst dst 目标矩形（像素，nullptr 表示 (0,0,纹理宽,纹理高)）
         * @param mod mod 颜色调制（替代 SDL_SetTextureColorMod/AlphaMod）
         */
        void DrawTexture(RenderLayer layer, float depth, TextureResourceResult *texture,
                         const SDL_FRect *src, const SDL_FRect *dst,
                         const SDL_Color &mod = {255, 255, 255, 255},
                         SDL_GPUGraphicsPipeline *pipeline = nullptr,
                         SDL_GPUSampler *sampler = nullptr);

        /**
         * Queue a texture region rotated around a center point, with
         * optional flipping (replaces SDL_RenderTextureRotated).
         * 将绕中心点旋转的纹理区域绘制命令入队，可选翻转
         * （替代 SDL_RenderTextureRotated）。
         * @param layer layer 命令所属渲染层
         * @param depth depth 层内深度
         * @param texture texture 要绘制的纹理
         * @param src src 源矩形（像素，nullptr 表示整个纹理）
         * @param dst dst 目标矩形（像素）
         * @param angleDegrees angleDegrees 顺时针角度（度）
         * @param center center 旋转中心（像素，nullptr 表示 dst 中心）
         * @param flip flip FLIP_HORIZONTAL/FLIP_VERTICAL 的按位或
         * @param mod mod 颜色调制
         */
        void DrawTextureRotated(RenderLayer layer, float depth, const TextureResourceResult *texture,
                                const SDL_FRect *src, const SDL_FRect *dst,
                                double angleDegrees, const SDL_FPoint *center, Uint8 flip,
                                const SDL_Color &mod = {255, 255, 255, 255},
                                SDL_GPUGraphicsPipeline *pipeline = nullptr,
                                SDL_GPUSampler *sampler = nullptr);

        /**
         * Queue a rectangle filled with a solid color
         * (replaces SDL_RenderFillRect + SDL_SetRenderDrawColor).
         * 将纯色填充矩形的命令入队（替代 SDL_RenderFillRect +
         * SDL_SetRenderDrawColor）。
         * @param layer layer 命令所属渲染层
         * @param depth depth 层内深度
         * @param rect rect 目标矩形（像素）
         * @param color color 填充颜色
         */
        void FillRect(RenderLayer layer, float depth, const SDL_FRect *rect, const SDL_Color &color);

        /**
         * Queue a 1-pixel rectangle outline with a solid color
         * (replaces SDL_RenderRect).
         * 将 1 像素矩形边框的命令入队（替代 SDL_RenderRect）。
         * @param layer layer 命令所属渲染层
         * @param depth depth 层内深度
         * @param rect rect 目标矩形（像素）
         * @param color color 边框颜色
         */
        void DrawRect(RenderLayer layer, float depth, const SDL_FRect *rect, const SDL_Color &color);

        /**
         * Queue a 1-pixel line with a solid color (replaces SDL_RenderLine).
         * 将 1 像素线段的命令入队（替代 SDL_RenderLine）。
         * @param layer layer 命令所属渲染层
         * @param depth depth 层内深度
         * @param color color 线段颜色
         */
        void DrawLine(RenderLayer layer, float depth, float x1, float y1, float x2, float y2,
                      const SDL_Color &color);

        /**
         * Queue a single pixel with a solid color (replaces SDL_RenderPoint).
         * 将单个像素的命令入队（替代 SDL_RenderPoint）。
         * @param layer layer 命令所属渲染层
         * @param depth depth 层内深度
         * @param color color 像素颜色
         */
        void DrawPoint(RenderLayer layer, float depth, float x, float y, const SDL_Color &color);
    };
}
