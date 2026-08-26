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
#include "GpuFrameState.h"

#include "core/log/LogCat.h"

namespace glimmer {
    bool GpuFrameState::BeginFrame(SDL_GPUDevice *device, SDL_Window *window) {
        if (device == nullptr) {
            return false;
        }
        commandBuffer_ = SDL_AcquireGPUCommandBuffer(device);
        if (commandBuffer_ == nullptr) {
            LogCat::w(std::source_location::current(), "SDL_AcquireGPUCommandBuffer failed: ", SDL_GetError());
            return false;
        }
        swapchainTexture_ = nullptr;
        if (!SDL_AcquireGPUSwapchainTexture(commandBuffer_, window, &swapchainTexture_, &swapchainWidth_,
                                            &swapchainHeight_)) {
            LogCat::w(std::source_location::current(), "SDL_AcquireGPUSwapchainTexture failed: ", SDL_GetError());
            SDL_CancelGPUCommandBuffer(commandBuffer_);
            commandBuffer_ = nullptr;
            return false;
        }
        if (swapchainTexture_ == nullptr) {
            //Too many frames in flight or window minimized: submit later, but skip drawing.
            //帧在飞行中过多或窗口最小化：稍后仍提交，但跳过绘制。
            frameActive_ = false;
            return false;
        }
        frameActive_ = true;
        return true;
    }

    bool GpuFrameState::SubmitFrame() {
        if (commandBuffer_ == nullptr) {
            return false;
        }
        const bool result = SDL_SubmitGPUCommandBuffer(commandBuffer_);
        if (!result) {
            LogCat::w(std::source_location::current(), "SDL_SubmitGPUCommandBuffer failed: ", SDL_GetError());
        }
        commandBuffer_ = nullptr;
        swapchainTexture_ = nullptr;
        frameActive_ = false;
        return result;
    }

    void GpuFrameState::Invalidate() {
        frameActive_ = false;
    }
}
