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
#include "LightMapTexture.h"

#include <algorithm>
#include <cmath>
#include <cstring>

#include "core/log/LogCat.h"
#include "core/math/TileVector2D.h"
#include "core/world/AmbientLight.h"
#include "core/world/LightBuffer.h"


void glimmer::LightMapTexture::EnsureTexture(const Uint32 width, const Uint32 height) {
    if (width == 0 || height == 0) {
        return;
    }
    if (texture_ != nullptr && width_ == width && height_ == height) {
        return;
    }
    if (texture_ != nullptr) {
        SDL_ReleaseGPUTexture(device_, texture_);
        texture_ = nullptr;
    }
    SDL_GPUTextureCreateInfo info = {};
    info.type = SDL_GPU_TEXTURETYPE_2D;
    info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    info.width = width;
    info.height = height;
    info.layer_count_or_depth = 1;
    info.num_levels = 1;
    info.sample_count = SDL_GPU_SAMPLECOUNT_1;
    info.props = 0;
    texture_ = SDL_CreateGPUTexture(device_, &info);
    if (texture_ == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_CreateGPUTexture failed: ", SDL_GetError());
        width_ = 0;
        height_ = 0;
        return;
    }
    width_ = width;
    height_ = height;
}

void glimmer::LightMapTexture::Update(SDL_GPUDevice *device, const LightBuffer *lightBuffer,
                                      const AmbientLight *ambient,
                                      const int originTileX, const int originTileY,
                                      const Uint32 sizeX, const Uint32 sizeY, const bool fullBright) {
    device_ = device;
    if (device_ == nullptr || sizeX == 0 || sizeY == 0) {
        return;
    }
    const uint64_t revision = lightBuffer != nullptr ? lightBuffer->GetRevision() : 0;
    const float ambientR = ambient != nullptr ? static_cast<float>(ambient->color.r) / 255.0F : 0.0F;
    const float ambientG = ambient != nullptr ? static_cast<float>(ambient->color.g) / 255.0F : 0.0F;
    const float ambientB = ambient != nullptr ? static_cast<float>(ambient->color.b) / 255.0F : 0.0F;
    //Quantize the continuous intensity so the light map is only rebuilt when
    //it actually crosses a 1/255 step instead of every frame.
    //量化连续强度，使光照贴图仅在强度真正跨越 1/255 步长时重建，而非每帧重建。
    const float ambientA = ambient != nullptr ? std::round(ambient->intensity * 255.0F) / 255.0F : 0.0F;
    if (lastRevision_ == revision && lastOriginX_ == originTileX && lastOriginY_ == originTileY &&
        lastSizeX_ == sizeX && lastSizeY_ == sizeY && lastFullBright_ == fullBright &&
        lastAmbient_[0] == ambientR && lastAmbient_[1] == ambientG && lastAmbient_[2] == ambientB &&
        lastAmbient_[3] == ambientA) {
        dirty_ = false;
        return;
    }
    EnsureTexture(sizeX, sizeY);
    if (texture_ == nullptr) {
        return;
    }
    lastRevision_ = revision;
    lastOriginX_ = originTileX;
    lastOriginY_ = originTileY;
    lastSizeX_ = sizeX;
    lastSizeY_ = sizeY;
    lastFullBright_ = fullBright;
    lastAmbient_[0] = ambientR;
    lastAmbient_[1] = ambientG;
    lastAmbient_[2] = ambientB;
    lastAmbient_[3] = ambientA;
    pixelBuffer_.resize(static_cast<size_t>(sizeX) * static_cast<size_t>(sizeY) * 4);
    for (Uint32 row = 0; row < sizeY; ++row) {
        //Texture row 0 is the top of the screen; world +Y points up, so rows
        //walk downwards from the highest tile y.
        //纹理第 0 行是屏幕顶部；世界 Y 轴向上，因此行从最高的瓦片 y 向下遍历。
        const int tileY = originTileY + static_cast<int>(sizeY - 1 - row);
        for (Uint32 col = 0; col < sizeX; ++col) {
            const int tileX = originTileX + static_cast<int>(col);
            Uint8 *pixel = pixelBuffer_.data() + (static_cast<size_t>(row) * sizeX + col) * 4;
            if (fullBright) {
                pixel[0] = 255;
                pixel[1] = 255;
                pixel[2] = 255;
                pixel[3] = 255;
                continue;
            }
            const TileVector2D tile(tileX, tileY);
            const Color *light = lightBuffer != nullptr
                                     ? lightBuffer->GetFinalLightColor(tile)
                                     : nullptr;
            const float srcR = light != nullptr ? static_cast<float>(light->r) / 255.0F : 0.0F;
            const float srcG = light != nullptr ? static_cast<float>(light->g) / 255.0F : 0.0F;
            const float srcB = light != nullptr ? static_cast<float>(light->b) / 255.0F : 0.0F;
            const float srcA = light != nullptr ? static_cast<float>(light->a) / 255.0F : 0.0F;
            const float sky = lightBuffer != nullptr ? lightBuffer->GetSkyVisibility(tile) : 0.0F;
            const float ambScale = ambientA * sky;
            const float totalR = srcR * srcA + ambientR * ambScale;
            const float totalG = srcG * srcA + ambientG * ambScale;
            const float totalB = srcB * srcA + ambientB * ambScale;
            const float totalA = std::max(srcA, ambScale);
            if (totalA <= 0.0F) {
                pixel[0] = 0;
                pixel[1] = 0;
                pixel[2] = 0;
                pixel[3] = 0;
            } else {
                pixel[0] = static_cast<Uint8>(std::min(255.0F, totalR / totalA * 255.0F));
                pixel[1] = static_cast<Uint8>(std::min(255.0F, totalG / totalA * 255.0F));
                pixel[2] = static_cast<Uint8>(std::min(255.0F, totalB / totalA * 255.0F));
                pixel[3] = static_cast<Uint8>(std::min(255.0F, totalA * 255.0F));
            }
        }
    }
    dirty_ = true;
}

void glimmer::LightMapTexture::Upload(SDL_GPUCommandBuffer *commandBuffer) {
    if (commandBuffer == nullptr || texture_ == nullptr || pixelBuffer_.empty() || !dirty_) {
        return;
    }
    const auto dataSize = static_cast<Uint32>(pixelBuffer_.size());
    if (transferBuffer_ == nullptr || transferBufferSize_ < dataSize) {
        if (transferBuffer_ != nullptr) {
            SDL_ReleaseGPUTransferBuffer(device_, transferBuffer_);
            transferBuffer_ = nullptr;
        }
        SDL_GPUTransferBufferCreateInfo info = {};
        info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        info.size = dataSize;
        info.props = 0;
        transferBuffer_ = SDL_CreateGPUTransferBuffer(device_, &info);
        transferBufferSize_ = transferBuffer_ != nullptr ? dataSize : 0;
    }
    if (transferBuffer_ == nullptr) {
        return;
    }
    void *mapped = SDL_MapGPUTransferBuffer(device_, transferBuffer_, false);
    if (mapped == nullptr) {
        return;
    }
    std::memcpy(mapped, pixelBuffer_.data(), dataSize);
    SDL_UnmapGPUTransferBuffer(device_, transferBuffer_);

    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);
    if (copyPass == nullptr) {
        return;
    }
    SDL_GPUTextureTransferInfo source = {};
    source.transfer_buffer = transferBuffer_;
    source.offset = 0;
    source.pixels_per_row = width_;
    source.rows_per_layer = height_;
    SDL_GPUTextureRegion destination = {};
    destination.texture = texture_;
    destination.mip_level = 0;
    destination.layer = 0;
    destination.x = 0;
    destination.y = 0;
    destination.z = 0;
    destination.w = width_;
    destination.h = height_;
    destination.d = 1;
    SDL_UploadToGPUTexture(copyPass, &source, &destination, false);
    SDL_EndGPUCopyPass(copyPass);
    dirty_ = false;
}

glimmer::LightMapTexture::~LightMapTexture() {
    if (transferBuffer_ != nullptr) {
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer_);
        transferBuffer_ = nullptr;
    }
    if (texture_ != nullptr) {
        SDL_ReleaseGPUTexture(device_, texture_);
        texture_ = nullptr;
    }
}

SDL_GPUTexture *glimmer::LightMapTexture::GetTexture() const {
    return texture_;
}

Uint32 glimmer::LightMapTexture::GetWidth() const {
    return width_;
}

Uint32 glimmer::LightMapTexture::GetHeight() const {
    return height_;
}
