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

#include "core/gpu/UniformBlock.h"
#include "ResourceResult.h"
#include "SDL3/SDL_gpu.h"

namespace glimmer {
    class UniformBlockResourceResult;

    /**
    * PipelineUniformBlock
    * 管线关联的 uniform 块及其每管线元数据（阶段、binding）。
    *
    * block 指向共享的编译块；stage 与 binding 是每管线属性，因此多管线
    * 共享同一个块时不会互相覆盖。
    */
    struct PipelineUniformBlock {
        std::shared_ptr<UniformBlockResourceResult> block;
        UniformBlockStage stage = UniformBlockStage::Fragment;
        uint32_t binding = 0;
    };

    /**
     * GPUPipelineResourceResult
     * GPU 管线资源结果
     */
    class GPUPipelineResourceResult : public ResourceResult<SDL_GPUGraphicsPipeline> {
        SDL_GPUDevice *device_ = nullptr;
        std::vector<PipelineUniformBlock> uniformBlocks_;

    protected:
        void DestroyResourceImpl(SDL_GPUGraphicsPipeline *resource) override;

    public:
        void SetDevice(SDL_GPUDevice *device);

        /**
         * AddUniformBlock
         * 追加一个管线关联的 Uniform 块。管线强持有该块，保证其生命周期与管线一致。
         * @param uniformBlock uniformBlock 管线关联的 Uniform 块及其阶段与绑定槽
         */
        void AddUniformBlock(PipelineUniformBlock uniformBlock);

        /**
         * GetUniformBlocks
         * 获取管线关联的全部 Uniform 块
         */
        [[nodiscard]] const std::vector<PipelineUniformBlock> *GetUniformBlocks() const;

        ~GPUPipelineResourceResult() override;
    };
}
