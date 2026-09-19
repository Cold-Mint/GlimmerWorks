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
#include "PassUtils.h"

#include <memory>
#include <vector>

#include "core/gpu/UniformBlock.h"
#include "core/mod/resourcePack/GPUPipelineResourceResult.h"
#include "core/mod/resourcePack/UniformBlockResourceResult.h"


void glimmer::ClearRenderTarget(SDL_GPUCommandBuffer *commandBuffer, SDL_GPUTexture *targetTexture) {
    if (commandBuffer == nullptr || targetTexture == nullptr) {
        return;
    }
    SDL_GPUColorTargetInfo colorTarget = {};
    colorTarget.texture = targetTexture;
    colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;
    colorTarget.clear_color = {0.0F, 0.0F, 0.0F, 1.0F};

    SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTarget, 1, nullptr);
    if (renderPass == nullptr) {
        return;
    }
    SDL_EndGPURenderPass(renderPass);
}

void glimmer::FillAndPushUniformBlock(
    SDL_GPUCommandBuffer *commandBuffer,
    const std::vector<PipelineUniformBlock> *uniformBlocks,
    const UniformInjectContext &ctx,
    std::vector<uint8_t> &stagingBuffer) {
    if (uniformBlocks == nullptr || uniformBlocks->empty()) {
        stagingBuffer.clear();
        return;
    }
    for (const PipelineUniformBlock &uniformBlock: *uniformBlocks) {
        if (uniformBlock.block == nullptr) {
            continue;
        }
        const UniformBlockResourceResult *uniformBlockResourceResultPtr = uniformBlock.block.get();
        if (uniformBlockResourceResultPtr == nullptr) {
            continue;
        }
        const CompiledUniformBlock *compiledUniformBlock = uniformBlockResourceResultPtr->GetResource();
        if (compiledUniformBlock == nullptr) {
            continue;
        }
        compiledUniformBlock->Fill(ctx, stagingBuffer);
        if (uniformBlock.stage == UniformBlockStage::Vertex) {
            SDL_PushGPUVertexUniformData(commandBuffer, uniformBlock.binding,
                                         stagingBuffer.data(), stagingBuffer.size());
        } else {
            SDL_PushGPUFragmentUniformData(commandBuffer, uniformBlock.binding,
                                           stagingBuffer.data(), stagingBuffer.size());
        }
    }
}
