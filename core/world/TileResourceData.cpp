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
#include "TileResourceData.h"

#include <utility>

#include "core/gpu/RenderCommand.h"


glimmer::TextureResourceResult *glimmer::TileResourceData::GetTexture() const {
    if (textureResult_ == nullptr) {
        return nullptr;
    }
    return textureResult_.get();
}

glimmer::AudioResourceResult *glimmer::TileResourceData::GetBreakSFX() const {
    if (breakSFXResult_ == nullptr) {
        return nullptr;
    }
    return breakSFXResult_.get();
}

glimmer::AudioResourceResult *glimmer::TileResourceData::GetPlaceSFX() const {
    if (placeSFXResult_ == nullptr) {
        return nullptr;
    }
    return placeSFXResult_.get();
}

glimmer::AudioResourceResult *glimmer::TileResourceData::GetPrimaryActionSFX() const {
    if (primaryActionSFXResult_ == nullptr) {
        return nullptr;
    }
    return primaryActionSFXResult_.get();
}

glimmer::AudioResourceResult *glimmer::TileResourceData::GetSecondaryActionSFX() const {
    if (secondaryActionSFXResult_ == nullptr) {
        return nullptr;
    }
    return secondaryActionSFXResult_.get();
}

const glimmer::ResourceRef *glimmer::TileResourceData::GetTextureRef() const {
    return &textureRef_;
}

const std::vector<glimmer::ItemTagResource> &glimmer::TileResourceData::GetTags() const {
    return tags_;
}

SDL_GPUGraphicsPipeline *glimmer::TileResourceData::GetPipeline() const {
    if (pipeline_ == nullptr) {
        return nullptr;
    }
    return pipeline_->GetResource();
}

SDL_GPUSampler *glimmer::TileResourceData::GetSampler() const {
    if (sampler_ == nullptr) {
        return nullptr;
    }
    return sampler_->GetResource();
}

const std::vector<glimmer::PipelineUniformBlock> *glimmer::TileResourceData::GetUniformBlocks() const {
    if (pipeline_ == nullptr) {
        return {};
    }
    return pipeline_->GetUniformBlocks();
}


void glimmer::TileResourceData::SetTexture(const std::shared_ptr<TextureResourceResult> &textureResult) {
    this->textureResult_ = textureResult;
}

void glimmer::TileResourceData::SetTextureRef(const ResourceRef &textureRef) {
    this->textureRef_ = textureRef;
}

void glimmer::TileResourceData::SetBreakSFX(const std::shared_ptr<AudioResourceResult> &breakSFXResult) {
    this->breakSFXResult_ = breakSFXResult;
}

void glimmer::TileResourceData::SetPlaceSFX(const std::shared_ptr<AudioResourceResult> &placeSFXResult) {
    this->placeSFXResult_ = placeSFXResult;
}

void glimmer::TileResourceData::SetPrimaryActionSFX(
    const std::shared_ptr<AudioResourceResult> &primaryActionSFXResult) {
    this->primaryActionSFXResult_ = primaryActionSFXResult;
}

void glimmer::TileResourceData::SetSecondaryActionSFX(
    const std::shared_ptr<AudioResourceResult> &secondaryActionSFXResult) {
    this->secondaryActionSFXResult_ = secondaryActionSFXResult;
}

void glimmer::TileResourceData::SetTags(const std::vector<ItemTagResource> &tags) {
    this->tags_ = tags;
}

void glimmer::TileResourceData::SetPipeline(std::shared_ptr<GPUPipelineResourceResult> pipeline) {
    pipeline_ = std::move(pipeline);
}

void glimmer::TileResourceData::SetSampler(std::shared_ptr<GPUSamplerResourceResult> sampler) {
    sampler_ = std::move(sampler);
}

void glimmer::TileResourceData::SetAttachmentTexture(const TileAttachmentDirection direction,
                                                     const std::shared_ptr<TextureResourceResult> &textureResult) {
    const auto index = static_cast<size_t>(direction);
    if (index < attachmentTextureResults_.size()) {
        attachmentTextureResults_[index] = textureResult;
    }
}

glimmer::TextureResourceResult *glimmer::TileResourceData::GetResolvedTexture(const TileAttachmentDirection direction,
                                                                              Uint8 &outFlip) const {
    outFlip = FLIP_NONE;
    if (direction == TileAttachmentDirection::None) {
        return GetTexture();
    }

    //Prefer explicit textures for this direction.
    //优先使用该方向的显式纹理。
    if (TextureResourceResult *directionTexture = GetAttachmentTexture(direction); directionTexture != nullptr) {
        return directionTexture;
    }

    //Textures are drawn facing right. When attaching to the left side, if no explicit leftward texture exists, generate it by horizontally flipping the right-side texture (or default texture).
    //纹理按朝右绘制。左侧附着时若无显式左向纹理，则用右侧纹理（或默认纹理）水平翻转生成。
    if (direction == TileAttachmentDirection::Left) {
        outFlip = FLIP_HORIZONTAL;
        if (TextureResourceResult *rightTexture = GetAttachmentTexture(TileAttachmentDirection::Right); rightTexture !=
            nullptr) {
            return rightTexture;
        }
    }
    return GetTexture();
}

glimmer::TextureResourceResult *glimmer::TileResourceData::GetAttachmentTexture(
    const TileAttachmentDirection direction) const {
    const auto index = static_cast<size_t>(direction);
    if (index >= attachmentTextureResults_.size()) {
        return nullptr;
    }
    return attachmentTextureResults_[index].get();
}
