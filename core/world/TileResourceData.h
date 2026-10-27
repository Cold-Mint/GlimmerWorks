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
#include <SDL3/SDL_gpu.h>

#include <array>

#include "core/mod/Resource.h"
#include "core/mod/resourcePack/AudioResourceResult.h"
#include "core/mod/resourcePack/GPUPipelineResourceResult.h"
#include "core/mod/resourcePack/GPUSamplerResourceResult.h"
#include "core/mod/resourcePack/TextureResourceResult.h"
#include "core/world/TileAttachmentType.h"

namespace glimmer {
    class TileResourceData {
        ResourceRef textureRef_;
        std::vector<ItemTagResource> tags_;
        std::shared_ptr<GPUPipelineResourceResult> pipeline_ = nullptr;
        std::shared_ptr<GPUSamplerResourceResult> sampler_ = nullptr;
        std::shared_ptr<TextureResourceResult> textureResult_ = nullptr;
        std::shared_ptr<AudioResourceResult> breakSFXResult_ = nullptr;
        std::shared_ptr<AudioResourceResult> placeSFXResult_ = nullptr;
        std::shared_ptr<AudioResourceResult> primaryActionSFXResult_ = nullptr;
        std::shared_ptr<AudioResourceResult> secondaryActionSFXResult_ = nullptr;
        //附着各方向的纹理，索引为 TileAttachmentDirection 的底层值（1-5）。
        std::array<std::shared_ptr<TextureResourceResult>, 6> attachmentTextureResults_ = {};

        [[nodiscard]] TextureResourceResult *GetAttachmentTexture(TileAttachmentDirection direction) const;

    public:
        [[nodiscard]] TextureResourceResult *GetTexture() const;

        [[nodiscard]] AudioResourceResult *GetBreakSFX() const;

        [[nodiscard]] AudioResourceResult *GetPlaceSFX() const;

        [[nodiscard]] AudioResourceResult *GetPrimaryActionSFX() const;

        [[nodiscard]] AudioResourceResult *GetSecondaryActionSFX() const;

        [[nodiscard]] const ResourceRef *GetTextureRef() const;

        [[nodiscard]] const std::vector<ItemTagResource> &GetTags() const;

        [[nodiscard]] SDL_GPUGraphicsPipeline *GetPipeline() const;

        [[nodiscard]] SDL_GPUSampler *GetSampler() const;

        [[nodiscard]] const std::vector<PipelineUniformBlock> *GetUniformBlocks() const;

        void SetTexture(const std::shared_ptr<TextureResourceResult> &textureResult);

        void SetTextureRef(const ResourceRef &textureRef);

        void SetBreakSFX(const std::shared_ptr<AudioResourceResult> &breakSFXResult);

        void SetPlaceSFX(const std::shared_ptr<AudioResourceResult> &placeSFXResult);

        void SetPrimaryActionSFX(const std::shared_ptr<AudioResourceResult> &primaryActionSFXResult);

        void SetSecondaryActionSFX(const std::shared_ptr<AudioResourceResult> &secondaryActionSFXResult);

        void SetTags(const std::vector<ItemTagResource> &tags);

        void SetPipeline(std::shared_ptr<GPUPipelineResourceResult> pipeline);

        void SetSampler(std::shared_ptr<GPUSamplerResourceResult> sampler);

        void SetAttachmentTexture(TileAttachmentDirection direction,
                                  const std::shared_ptr<TextureResourceResult> &textureResult);


        /**
         * GetResolvedTexture
         * 按附着方向解析最终使用的纹理与翻转标志：优先方向纹理，回退到默认纹理。
         * 约定A：纹理按朝右绘制。左侧附着时优先显式左向纹理（不翻转），
         * 否则用右侧纹理（或默认纹理）水平翻转生成。
         * @param direction 附着方向
         * @param outFlip 输出翻转标志（FLIP_HORIZONTAL/FLIP_VERTICAL 的按位或）
         * @return 解析后的纹理，可能为 nullptr
         */
        [[nodiscard]] TextureResourceResult *
        GetResolvedTexture(TileAttachmentDirection direction, Uint8 &outFlip) const;
    };
}
