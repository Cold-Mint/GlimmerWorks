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
#include <SDL3/SDL_pixels.h>

namespace glimmer {
    /**
     * GpuRenderPassState
     * GPU 渲染通道状态
     *
     * Tracks the currently active SDL_GPURenderPass and provides helpers to
     * begin/end passes on a given target texture.
     * 跟踪当前活动的 SDL_GPURenderPass，并提供在给定目标纹理上开始/结束
     * 通道的辅助函数。
     */
    class GpuRenderPassState {
        SDL_GPURenderPass *renderPass_ = nullptr;
        SDL_GPUTexture *currentTarget_ = nullptr;

    public:
        GpuRenderPassState() = default;

        /**
         * Begin a render pass on the given target if none is active.
         * 若当前没有活动通道，则在指定目标上开始一个渲染通道。
         * @param commandBuffer commandBuffer 当前帧命令缓冲
         * @param target target 目标纹理
         * @param clear clear 为 true 时用 clearColor 清屏，否则加载已有内容
         * @param clearColor clearColor 清屏颜色
         */
        void Begin(SDL_GPUCommandBuffer *commandBuffer, SDL_GPUTexture *target, bool clear,
                   const SDL_Color &clearColor);

        /**
         * End the active render pass (if any).
         * 结束当前活动的渲染通道（若有）。
         */
        void End();

        /**
         * End any active pass and prepare to render to a new target.
         * 结束任何活动通道，并准备渲染到新目标。
         */
        void TransitionTo(SDL_GPUCommandBuffer *commandBuffer, SDL_GPUTexture *target, bool clear,
                          const SDL_Color &clearColor) {
            End();
            Begin(commandBuffer, target, clear, clearColor);
        }

        [[nodiscard]] bool IsActive() const { return renderPass_ != nullptr; }

        [[nodiscard]] SDL_GPURenderPass *GetRenderPass() const { return renderPass_; }

        [[nodiscard]] SDL_GPUTexture *GetCurrentTarget() const { return currentTarget_; }
    };
}
