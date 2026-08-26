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
#include "GpuSpriteBatcher.h"

#include "GpuContext.h"
#include "GpuTexture.h"
#include "core/log/LogCat.h"

namespace glimmer {
    bool GpuSpriteBatcher::Init(GpuContext *context) {
        SDL_GPUDevice *device = context != nullptr ? context->GetDevice() : nullptr;
        if (device == nullptr) {
            return false;
        }

        SDL_GPUBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
        bufferCreateInfo.size = INITIAL_VERTEX_CAPACITY * sizeof(SpriteVertex);
        bufferCreateInfo.props = 0;
        vertexBuffer_ = SDL_CreateGPUBuffer(device, &bufferCreateInfo);
        if (vertexBuffer_ == nullptr) {
            LogCat::e(std::source_location::current(), "SDL_CreateGPUBuffer failed: ", SDL_GetError());
            return false;
        }
        vertexBufferCapacity_ = INITIAL_VERTEX_CAPACITY;

        SDL_Surface *whiteSurface = SDL_CreateSurface(1, 1, SDL_PIXELFORMAT_ABGR8888);
        if (whiteSurface == nullptr) {
            LogCat::e(std::source_location::current(), "SDL_CreateSurface failed: ", SDL_GetError());
            Shutdown(device);
            return false;
        }
        SDL_FillSurfaceRect(whiteSurface, nullptr, SDL_MapSurfaceRGBA(whiteSurface, 255, 255, 255, 255));
        whiteTexture_.reset(context->CreateTextureFromSurface(whiteSurface));
        SDL_DestroySurface(whiteSurface);
        if (whiteTexture_ == nullptr) {
            LogCat::e(std::source_location::current(), "Failed to create white texture");
            Shutdown(device);
            return false;
        }
        return true;
    }

    void GpuSpriteBatcher::Shutdown(SDL_GPUDevice *device) {
        whiteTexture_.reset();
        if (vertexBuffer_ != nullptr && device != nullptr) {
            SDL_ReleaseGPUBuffer(device, vertexBuffer_);
            vertexBuffer_ = nullptr;
        }
        vertexBufferCapacity_ = 0;
    }

    void GpuSpriteBatcher::GrowBuffer(SDL_GPUDevice *device, const size_t requiredVertexCount) {
        if (vertexBuffer_ != nullptr && vertexBufferCapacity_ >= requiredVertexCount) {
            return;
        }
        Uint32 newCapacity = vertexBufferCapacity_ == 0 ? INITIAL_VERTEX_CAPACITY : vertexBufferCapacity_;
        while (newCapacity < requiredVertexCount) {
            newCapacity *= 2;
        }
        SDL_GPUBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
        bufferCreateInfo.size = newCapacity * sizeof(SpriteVertex);
        bufferCreateInfo.props = 0;
        SDL_GPUBuffer *newBuffer = SDL_CreateGPUBuffer(device, &bufferCreateInfo);
        if (newBuffer == nullptr) {
            LogCat::w(std::source_location::current(), "Failed to grow vertex buffer: ", SDL_GetError());
            return;
        }
        if (vertexBuffer_ != nullptr) {
            SDL_ReleaseGPUBuffer(device, vertexBuffer_);
        }
        vertexBuffer_ = newBuffer;
        vertexBufferCapacity_ = newCapacity;
    }

    void GpuSpriteBatcher::ExpandRange(const RenderCommand *commands, const size_t count,
                                       const GpuTexture *whiteTexture,
                                       std::vector<SpriteVertex> &vertices, std::vector<DrawRun> &runs) {
        SDL_GPUTexture *currentTexture = nullptr;
        SDL_GPUTexture *fallbackTexture = whiteTexture != nullptr ? whiteTexture->GetGpuTexture() : nullptr;
        for (size_t i = 0; i < count; ++i) {
            const RenderCommand &command = commands[i];
            SDL_GPUTexture *commandTexture = command.texture != nullptr
                                                 ? command.texture->GetGpuTexture()
                                                 : fallbackTexture;
            if (commandTexture == nullptr) {
                continue;
            }
            if (currentTexture != commandTexture) {
                currentTexture = commandTexture;
                runs.push_back({commandTexture, static_cast<Uint32>(vertices.size()), 0});
            }
            const SpriteVertex &topLeft = command.corners[0];
            const SpriteVertex &topRight = command.corners[1];
            const SpriteVertex &bottomLeft = command.corners[2];
            const SpriteVertex &bottomRight = command.corners[3];
            vertices.push_back(topLeft);
            vertices.push_back(topRight);
            vertices.push_back(bottomLeft);
            vertices.push_back(topRight);
            vertices.push_back(bottomRight);
            vertices.push_back(bottomLeft);
            runs.back().vertexCount += 6;
        }
    }

    GpuSpriteBatcher::Batch GpuSpriteBatcher::Build(const std::vector<RenderCommand> &commands,
                                                    const size_t splitIndex) {
        Batch batch;
        batch.vertices.reserve(commands.size() * 6);
        ExpandRange(commands.data(), splitIndex, whiteTexture_.get(), batch.vertices, batch.worldRuns);
        ExpandRange(commands.data() + splitIndex, commands.size() - splitIndex, whiteTexture_.get(), batch.vertices,
                    batch.overlayRuns);
        return batch;
    }

    bool GpuSpriteBatcher::Upload(SDL_GPUDevice *device, SDL_GPUCommandBuffer *commandBuffer,
                                  const std::vector<SpriteVertex> &vertices) {
        if (device == nullptr || commandBuffer == nullptr) {
            return false;
        }
        GrowBuffer(device, vertices.size());
        if (vertexBuffer_ == nullptr || vertexBufferCapacity_ < vertices.size()) {
            return false;
        }
        if (vertices.empty()) {
            return true;
        }
        const Uint32 dataSize = static_cast<Uint32>(vertices.size() * sizeof(SpriteVertex));
        SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {};
        transferBufferCreateInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transferBufferCreateInfo.size = dataSize;
        transferBufferCreateInfo.props = 0;
        SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferBufferCreateInfo);
        if (transferBuffer == nullptr) {
            LogCat::w(std::source_location::current(), "SDL_CreateGPUTransferBuffer failed: ", SDL_GetError());
            return false;
        }
        void *mapped = SDL_MapGPUTransferBuffer(device, transferBuffer, false);
        if (mapped == nullptr) {
            SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
            LogCat::w(std::source_location::current(), "SDL_MapGPUTransferBuffer failed: ", SDL_GetError());
            return false;
        }
        SDL_memcpy(mapped, vertices.data(), dataSize);
        SDL_UnmapGPUTransferBuffer(device, transferBuffer);
        SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);
        if (copyPass == nullptr) {
            SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
            return false;
        }
        SDL_GPUTransferBufferLocation location = {};
        location.transfer_buffer = transferBuffer;
        location.offset = 0;
        SDL_GPUBufferRegion region = {};
        region.buffer = vertexBuffer_;
        region.offset = 0;
        region.size = dataSize;
        SDL_UploadToGPUBuffer(copyPass, &location, &region, false);
        SDL_EndGPUCopyPass(copyPass);
        SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
        return true;
    }

    void GpuSpriteBatcher::DrawRuns(SDL_GPURenderPass *renderPass, const std::vector<DrawRun> &runs,
                                    SDL_GPUSampler *sampler) {
        if (renderPass == nullptr) {
            return;
        }
        for (const DrawRun &run: runs) {
            SDL_GPUTextureSamplerBinding textureBinding = {};
            textureBinding.texture = run.texture;
            textureBinding.sampler = sampler;
            SDL_BindGPUFragmentSamplers(renderPass, 0, &textureBinding, 1);
            SDL_DrawGPUPrimitives(renderPass, run.vertexCount, 1, run.firstVertex, 0);
        }
    }
}
