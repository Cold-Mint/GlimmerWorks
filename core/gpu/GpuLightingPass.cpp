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
#include "GpuLightingPass.h"

#include "GpuContext.h"
#include "GpuTexture.h"
#include "core/log/LogCat.h"

namespace glimmer {
    void GpuLightingPass::SetLightMap(const int width, const int height, const Uint8 *rgbaPixels,
                                      const LightMapParams &params) {
        if (width <= 0 || height <= 0) {
            return;
        }
        lightMapPending_ = true;
        lightMapParams_ = params;
        if (rgbaPixels == nullptr) {
            return;
        }
        const size_t dataSize = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;
        if (lightMapPixels_.size() != dataSize) {
            lightMapPixels_.resize(dataSize);
        }
        SDL_memcpy(lightMapPixels_.data(), rgbaPixels, dataSize);
        lightMapWidth_ = width;
        lightMapHeight_ = height;
        lightMapDirty_ = true;
    }

    bool GpuLightingPass::ConsumeRequest() {
        const bool requested = lightMapPending_;
        lightMapPending_ = false;
        return requested;
    }

    bool GpuLightingPass::Upload(SDL_GPUCommandBuffer *commandBuffer, GpuContext *context) {
        SDL_GPUDevice *device = context != nullptr ? context->GetDevice() : nullptr;
        if (device == nullptr || commandBuffer == nullptr) {
            return false;
        }
        if (!lightMapDirty_) {
            return lightMapTexture_ != nullptr && lightMapTexture_->IsValid();
        }
        lightMapDirty_ = false;
        if (lightMapTexture_ == nullptr || lightMapTexture_->w != lightMapWidth_ ||
            lightMapTexture_->h != lightMapHeight_) {
            lightMapTexture_.reset();
            lightMapTexture_.reset(context->CreateSampledTexture(static_cast<Uint32>(lightMapWidth_),
                                                                 static_cast<Uint32>(lightMapHeight_),
                                                                 SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM));
            if (lightMapTexture_ == nullptr) {
                LogCat::w(std::source_location::current(), "Failed to create light map texture");
                return false;
            }
        }
        const Uint32 dataSize = static_cast<Uint32>(lightMapPixels_.size());
        SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {};
        transferBufferCreateInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transferBufferCreateInfo.size = dataSize;
        transferBufferCreateInfo.props = 0;
        SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferBufferCreateInfo);
        if (transferBuffer == nullptr) {
            LogCat::w(std::source_location::current(), "SDL_CreateGPUTransferBuffer(light map) failed: ",
                      SDL_GetError());
            return false;
        }
        void *mapped = SDL_MapGPUTransferBuffer(device, transferBuffer, false);
        if (mapped == nullptr) {
            SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
            LogCat::w(std::source_location::current(), "SDL_MapGPUTransferBuffer(light map) failed: ", SDL_GetError());
            return false;
        }
        SDL_memcpy(mapped, lightMapPixels_.data(), dataSize);
        SDL_UnmapGPUTransferBuffer(device, transferBuffer);
        SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);
        if (copyPass == nullptr) {
            SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
            LogCat::w(std::source_location::current(), "SDL_BeginGPUCopyPass(light map) failed: ", SDL_GetError());
            return false;
        }
        SDL_GPUTextureTransferInfo transferInfo = {};
        transferInfo.transfer_buffer = transferBuffer;
        transferInfo.offset = 0;
        transferInfo.pixels_per_row = static_cast<Uint32>(lightMapWidth_);
        transferInfo.rows_per_layer = static_cast<Uint32>(lightMapHeight_);
        SDL_GPUTextureRegion textureRegion = {};
        textureRegion.texture = lightMapTexture_->GetGpuTexture();
        textureRegion.mip_level = 0;
        textureRegion.layer = 0;
        textureRegion.x = 0;
        textureRegion.y = 0;
        textureRegion.w = static_cast<Uint32>(lightMapWidth_);
        textureRegion.h = static_cast<Uint32>(lightMapHeight_);
        textureRegion.d = 1;
        SDL_UploadToGPUTexture(copyPass, &transferInfo, &textureRegion, false);
        SDL_EndGPUCopyPass(copyPass);
        SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
        return true;
    }

    void GpuLightingPass::Draw(SDL_GPUCommandBuffer *commandBuffer, SDL_GPURenderPass *renderPass,
                               SDL_GPUGraphicsPipeline *lightingPipeline, SDL_GPUBuffer *unitQuadBuffer,
                               SDL_GPUSampler *linearSampler) {
        if (commandBuffer == nullptr || renderPass == nullptr || lightingPipeline == nullptr ||
            lightMapTexture_ == nullptr || !lightMapTexture_->IsValid() || unitQuadBuffer == nullptr) {
            return;
        }
        SDL_BindGPUGraphicsPipeline(renderPass, lightingPipeline);
        const float viewSize[2] = {1.0F, 1.0F};
        SDL_PushGPUVertexUniformData(commandBuffer, 0, viewSize, sizeof(viewSize));
        SDL_PushGPUFragmentUniformData(commandBuffer, 0, &lightMapParams_, sizeof(LightMapParams));
        SDL_GPUBufferBinding vertexBufferBinding = {};
        vertexBufferBinding.buffer = unitQuadBuffer;
        vertexBufferBinding.offset = 0;
        SDL_BindGPUVertexBuffers(renderPass, 0, &vertexBufferBinding, 1);
        SDL_GPUTextureSamplerBinding textureBinding = {};
        textureBinding.texture = lightMapTexture_->GetGpuTexture();
        textureBinding.sampler = linearSampler;
        SDL_BindGPUFragmentSamplers(renderPass, 0, &textureBinding, 1);
        SDL_DrawGPUPrimitives(renderPass, 6, 1, 0, 0);
    }
}
