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
#include <vector>

#include <SDL3/SDL_gpu.h>

namespace glimmer {
    class GPUPipelineResourceResult;
    class UniformInjectContext;

    /**
     * Clear a render target to solid black within a single clear-and-store
     * render pass. Called once every frame, before the world/UI passes, so the
     * previous frame's content never persists behind the RmlUi UI.
     * 在单次 clear/store 渲染通道内把渲染目标清为纯黑。每帧在世界/UI 通道
     * 之前调用一次，避免上一帧内容残留在 RmlUi UI 之后。
     */
    void ClearRenderTarget(SDL_GPUCommandBuffer *commandBuffer, SDL_GPUTexture *targetTexture);

    /**
     * Fill and push the uniform block of a fullscreen pass. Encapsulates
     * "fetch pipeline uniform block -> fill with ctx -> push", so any new
     * fullscreen/post-processing pass can reuse the same injection path.
     * 填充并推送一个全屏 pass 的 uniform 块。封装
     * "取管线 uniform 块 -> 用 ctx 填充 -> 推送"，使新的全屏/后处理 pass 复用同一注入路径。
     */
    void FillAndPushUniformBlock(SDL_GPUCommandBuffer *commandBuffer,
                                 const std::shared_ptr<GPUPipelineResourceResult> &pipeline,
                                 const UniformInjectContext &ctx,
                                 std::vector<uint8_t> &stagingBuffer);
}
