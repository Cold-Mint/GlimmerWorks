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

struct SDL_Window;

namespace glimmer {
    /**
     * GpuFrameState
     * GPU 帧状态
     *
     * Manages the per-frame SDL_GPU objects: the command buffer and the
     * swapchain texture. Keeps track of whether the frame is active and
     * the current swapchain size.
     * 管理每帧的 SDL_GPU 对象：命令缓冲和交换链纹理。跟踪帧是否处于活动
     * 状态以及当前交换链尺寸。
     */
    class GpuFrameState {
        SDL_GPUCommandBuffer *commandBuffer_ = nullptr;
        SDL_GPUTexture *swapchainTexture_ = nullptr;
        Uint32 swapchainWidth_ = 0;
        Uint32 swapchainHeight_ = 0;
        bool frameActive_ = false;

    public:
        GpuFrameState() = default;

        /**
         * Acquire a command buffer and the swapchain texture for the given window.
         * 为指定窗口获取命令缓冲和交换链纹理。
         * @param device device 已初始化的 GPU 设备
         * @param window window 目标窗口
         * @return true if a swapchain texture was acquired and drawing is possible.
         * 成功获取交换链纹理且可以绘制时返回 true。
         */
        bool BeginFrame(SDL_GPUDevice *device, SDL_Window *window);

        /**
         * Submit the command buffer and reset frame state.
         * 提交命令缓冲并重置帧状态。
         * @return the submit result.
         * 提交是否成功。
         */
        bool SubmitFrame();

        /**
         * Mark the frame as inactive (e.g. window minimized).
         * 将帧标记为不活跃（如窗口最小化）。
         */
        void Invalidate();

        [[nodiscard]] bool IsActive() const { return frameActive_; }

        [[nodiscard]] SDL_GPUCommandBuffer *GetCommandBuffer() const { return commandBuffer_; }

        [[nodiscard]] SDL_GPUTexture *GetSwapchainTexture() const { return swapchainTexture_; }

        [[nodiscard]] Uint32 GetSwapchainWidth() const { return swapchainWidth_; }

        [[nodiscard]] Uint32 GetSwapchainHeight() const { return swapchainHeight_; }
    };
}
