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
#include "GpuRenderPassState.h"

#include "core/log/LogCat.h"

namespace glimmer {
    void GpuRenderPassState::Begin(SDL_GPUCommandBuffer *commandBuffer, SDL_GPUTexture *target, const bool clear,
                                   const SDL_Color &clearColor) {
        if (renderPass_ != nullptr || target == nullptr || commandBuffer == nullptr) {
            return;
        }
        SDL_GPUColorTargetInfo colorTargetInfo = {};
        colorTargetInfo.texture = target;
        colorTargetInfo.mip_level = 0;
        colorTargetInfo.layer_or_depth_plane = 0;
        colorTargetInfo.load_op = clear ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
        colorTargetInfo.clear_color = {
            static_cast<float>(clearColor.r) / 255.0F,
            static_cast<float>(clearColor.g) / 255.0F,
            static_cast<float>(clearColor.b) / 255.0F,
            static_cast<float>(clearColor.a) / 255.0F
        };
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
        colorTargetInfo.cycle = false;
        renderPass_ = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, nullptr);
        if (renderPass_ == nullptr) {
            LogCat::w(std::source_location::current(), "SDL_BeginGPURenderPass failed: ", SDL_GetError());
        }
        currentTarget_ = target;
    }

    void GpuRenderPassState::End() {
        if (renderPass_ != nullptr) {
            SDL_EndGPURenderPass(renderPass_);
            renderPass_ = nullptr;
        }
        currentTarget_ = nullptr;
    }
}
