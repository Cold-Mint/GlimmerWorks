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
#include "ScenePass.h"

#include <cstddef>
#include <cstring>
#include <vector>

#include "core/gpu/RenderCommand.h"
#include "core/gpu/RenderFrameContext.h"
#include "core/gpu/RenderQueue.h"
#include "core/gpu/UniformBlock.h"
#include "core/log/LogCat.h"
#include "core/mod/resourcePack/GPUPipelineResourceResult.h"
#include "core/mod/resourcePack/GPUSamplerResourceResult.h"


glimmer::ScenePass::ScenePass(SDL_GPUDevice *device, SDL_Window *window,
                              std::shared_ptr<GPUPipelineResourceResult> defaultPipeline,
                              std::shared_ptr<GPUSamplerResourceResult> defaultSampler)
    : device_(device),
      window_(window),
      defaultPipeline_(std::move(defaultPipeline)),
      defaultSampler_(std::move(defaultSampler)) {
    EnsureSolidColorTexture();
}

glimmer::ScenePass::~ScenePass() {
    if (sceneTexture_ != nullptr) {
        SDL_ReleaseGPUTexture(device_, sceneTexture_);
    }
    if (solidColorTexture_ != nullptr) {
        SDL_ReleaseGPUTexture(device_, solidColorTexture_);
    }
    if (vertexBuffer_ != nullptr) {
        SDL_ReleaseGPUBuffer(device_, vertexBuffer_);
    }
    if (indexBuffer_ != nullptr) {
        SDL_ReleaseGPUBuffer(device_, indexBuffer_);
    }
    if (transferBuffer_ != nullptr) {
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer_);
    }
}

void glimmer::ScenePass::Prepare(RenderFrameContext &ctx) {
    EnsureSceneTexture(ctx);
}

void glimmer::ScenePass::Record(RenderFrameContext &ctx) {
    FlushScenePass(ctx);
}

void glimmer::ScenePass::EnsureSceneTexture(RenderFrameContext &ctx) {
    const Uint32 width = ctx.logicalWidth;
    const Uint32 height = ctx.logicalHeight;
    if (width == 0 || height == 0) {
        return;
    }
    if (sceneTexture_ != nullptr && sceneTextureWidth_ == width && sceneTextureHeight_ == height) {
        ctx.sceneTexture = sceneTexture_;
        ctx.sceneTextureWidth = sceneTextureWidth_;
        ctx.sceneTextureHeight = sceneTextureHeight_;
        return;
    }
    if (sceneTexture_ != nullptr) {
        SDL_ReleaseGPUTexture(device_, sceneTexture_);
        sceneTexture_ = nullptr;
    }
    SDL_GPUTextureCreateInfo info = {};
    info.type = SDL_GPU_TEXTURETYPE_2D;
    //The pipelines are created with the swapchain format as their color
    //target, so the offscreen target must use the same format or the scene
    //pass produces a black/garbage result.
    //管线以交换链格式作为颜色目标创建，因此离屏目标必须使用相同格式，
    //否则场景 pass 会输出黑色/错误结果。
    info.format = SDL_GetGPUSwapchainTextureFormat(device_, window_);
    info.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
    info.width = width;
    info.height = height;
    info.layer_count_or_depth = 1;
    info.num_levels = 1;
    info.sample_count = SDL_GPU_SAMPLECOUNT_1;
    info.props = 0;
    sceneTexture_ = SDL_CreateGPUTexture(device_, &info);
    if (sceneTexture_ == nullptr) {
        LogCat::w(std::source_location::current(), "sdl_create_gpu_texture_failed", "SDL_CreateGPUTexture failed: {}",
                  SDL_GetError());
        return;
    }
    LogCat::i("app_renderer_scene_texture_format", "Scene texture created: {}x{}, format={}",
              width, height, static_cast<int>(info.format));
    sceneTextureWidth_ = width;
    sceneTextureHeight_ = height;
    ctx.sceneTexture = sceneTexture_;
    ctx.sceneTextureWidth = width;
    ctx.sceneTextureHeight = height;
}

void glimmer::ScenePass::EnsureVertexBufferSize(const Uint32 size) {
    if (size <= vertexBufferSize_) {
        return;
    }
    Uint32 newSize = vertexBufferSize_ > 0 ? vertexBufferSize_ : 1;
    while (newSize < size) {
        newSize *= 2;
    }
    if (vertexBuffer_ != nullptr) {
        SDL_ReleaseGPUBuffer(device_, vertexBuffer_);
    }
    SDL_GPUBufferCreateInfo info = {};
    info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    info.size = newSize;
    vertexBuffer_ = SDL_CreateGPUBuffer(device_, &info);
    vertexBufferSize_ = vertexBuffer_ != nullptr ? newSize : 0;
}

void glimmer::ScenePass::EnsureIndexBufferSize(const Uint32 size) {
    if (size <= indexBufferSize_) {
        return;
    }
    Uint32 newSize = indexBufferSize_ > 0 ? indexBufferSize_ : 1;
    while (newSize < size) {
        newSize *= 2;
    }
    if (indexBuffer_ != nullptr) {
        SDL_ReleaseGPUBuffer(device_, indexBuffer_);
    }
    SDL_GPUBufferCreateInfo info = {};
    info.usage = SDL_GPU_BUFFERUSAGE_INDEX;
    info.size = newSize;
    indexBuffer_ = SDL_CreateGPUBuffer(device_, &info);
    indexBufferSize_ = indexBuffer_ != nullptr ? newSize : 0;
}

void glimmer::ScenePass::EnsureTransferBufferSize(const Uint32 size) {
    if (size <= transferBufferSize_) {
        return;
    }
    Uint32 newSize = transferBufferSize_ > 0 ? transferBufferSize_ : 1;
    while (newSize < size) {
        newSize *= 2;
    }
    if (transferBuffer_ != nullptr) {
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer_);
    }
    SDL_GPUTransferBufferCreateInfo info = {};
    info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    info.size = newSize;
    transferBuffer_ = SDL_CreateGPUTransferBuffer(device_, &info);
    transferBufferSize_ = transferBuffer_ != nullptr ? newSize : 0;
}

void glimmer::ScenePass::EnsureSolidColorTexture() {
    if (device_ == nullptr || solidColorTexture_ != nullptr) {
        return;
    }
    SDL_GPUTextureCreateInfo textureInfo = {};
    textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    textureInfo.width = 1;
    textureInfo.height = 1;
    textureInfo.layer_count_or_depth = 1;
    textureInfo.num_levels = 1;
    textureInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;
    textureInfo.props = 0;
    solidColorTexture_ = SDL_CreateGPUTexture(device_, &textureInfo);
    if (solidColorTexture_ == nullptr) {
        LogCat::w(std::source_location::current(), "sdl_create_gpu_texture_failed", "SDL_CreateGPUTexture failed: {}",
                  SDL_GetError());
        return;
    }
    constexpr Uint8 whitePixel[4] = {255, 255, 255, 255};
    SDL_GPUTransferBufferCreateInfo transferInfo = {};
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferInfo.size = sizeof(whitePixel);
    transferInfo.props = 0;
    SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(device_, &transferInfo);
    if (transferBuffer == nullptr) {
        LogCat::w(std::source_location::current(), "sdl_create_gpu_transfer_buffer_failed",
                  "SDL_CreateGPUTransferBuffer failed: {}", SDL_GetError());
        return;
    }
    void *mapped = SDL_MapGPUTransferBuffer(device_, transferBuffer, false);
    if (mapped != nullptr) {
        std::memcpy(mapped, whitePixel, sizeof(whitePixel));
        SDL_UnmapGPUTransferBuffer(device_, transferBuffer);
    }
    SDL_GPUCommandBuffer *uploadCommandBuffer = SDL_AcquireGPUCommandBuffer(device_);
    if (uploadCommandBuffer == nullptr) {
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
        return;
    }
    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(uploadCommandBuffer);
    if (copyPass == nullptr) {
        SDL_CancelGPUCommandBuffer(uploadCommandBuffer);
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
        return;
    }
    SDL_GPUTextureTransferInfo source = {};
    source.transfer_buffer = transferBuffer;
    source.offset = 0;
    source.pixels_per_row = 1;
    source.rows_per_layer = 1;
    SDL_GPUTextureRegion destination = {};
    destination.texture = solidColorTexture_;
    destination.mip_level = 0;
    destination.layer = 0;
    destination.w = 1;
    destination.h = 1;
    destination.d = 1;
    SDL_UploadToGPUTexture(copyPass, &source, &destination, false);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(uploadCommandBuffer);
    SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
}

void glimmer::ScenePass::FlushScenePass(RenderFrameContext &ctx) {
    SDL_GPUCommandBuffer *commandBuffer = ctx.commandBuffer;
    SDL_GPUTexture *targetTexture = ctx.sceneTexture;
    const Uint32 width = ctx.sceneTextureWidth;
    const Uint32 height = ctx.sceneTextureHeight;
    UniformInjectContext *injectContext = ctx.injectContext;
    RenderQueue *renderQueue = ctx.renderQueue;

    if (commandBuffer == nullptr || targetTexture == nullptr || renderQueue == nullptr) {
        return;
    }

    const bool hasCommands = !renderQueue->IsEmpty();

    if (hasCommands) {
        renderQueue->Sort();
        const std::vector<RenderCommand> &commands = renderQueue->GetCommands();
        vertexStaging_.clear();
        indexStaging_.clear();
        vertexStaging_.reserve(commands.size() * 4);
        indexStaging_.reserve(commands.size() * 6);
        for (const RenderCommand &command: commands) {
            const auto baseIndex = static_cast<Uint32>(vertexStaging_.size());
            vertexStaging_.insert(vertexStaging_.end(), command.corners, command.corners + 4);
            const Uint32 quadIndices[6] = {
                baseIndex + 0, baseIndex + 1, baseIndex + 2,
                baseIndex + 1, baseIndex + 3, baseIndex + 2
            };
            indexStaging_.insert(indexStaging_.end(), quadIndices, quadIndices + 6);
        }
        const auto vertexDataSize = static_cast<Uint32>(vertexStaging_.size() * sizeof(SpriteVertex));
        const auto indexDataSize = static_cast<Uint32>(indexStaging_.size() * sizeof(Uint32));
        EnsureVertexBufferSize(vertexDataSize);
        EnsureIndexBufferSize(indexDataSize);
        EnsureTransferBufferSize(vertexDataSize + indexDataSize);

        if (vertexBuffer_ != nullptr && indexBuffer_ != nullptr && transferBuffer_ != nullptr) {
            void *mapped = SDL_MapGPUTransferBuffer(device_, transferBuffer_, true);
            if (mapped != nullptr) {
                std::memcpy(mapped, vertexStaging_.data(), vertexDataSize);
                std::memcpy(static_cast<Uint8 *>(mapped) + vertexDataSize, indexStaging_.data(), indexDataSize);
                SDL_UnmapGPUTransferBuffer(device_, transferBuffer_);
            }

            SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);
            if (copyPass != nullptr) {
                SDL_GPUTransferBufferLocation vertexSource = {transferBuffer_, 0};
                SDL_GPUBufferRegion vertexDestination = {vertexBuffer_, 0, vertexDataSize};
                SDL_UploadToGPUBuffer(copyPass, &vertexSource, &vertexDestination, true);
                SDL_GPUTransferBufferLocation indexSource = {transferBuffer_, vertexDataSize};
                SDL_GPUBufferRegion indexDestination = {indexBuffer_, 0, indexDataSize};
                SDL_UploadToGPUBuffer(copyPass, &indexSource, &indexDestination, true);
                SDL_EndGPUCopyPass(copyPass);
            }
        }
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

    if (hasCommands && vertexBuffer_ != nullptr && indexBuffer_ != nullptr) {
        SDL_GPUBufferBinding vertexBinding = {vertexBuffer_, 0};
        SDL_BindGPUVertexBuffers(renderPass, 0, &vertexBinding, 1);
        SDL_GPUBufferBinding indexBinding = {indexBuffer_, 0};
        SDL_BindGPUIndexBuffer(renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

        const float viewSize[2] = {static_cast<float>(width), static_cast<float>(height)};
        SDL_PushGPUVertexUniformData(commandBuffer, 0, viewSize, sizeof(viewSize));

        const std::vector<RenderCommand> &commands = renderQueue->GetCommands();
        SDL_GPUGraphicsPipeline *defaultPipeline = defaultPipeline_->GetResource();
        SDL_GPUSampler *defaultSampler = defaultSampler_->GetResource();
        SDL_GPUGraphicsPipeline *currentPipeline = defaultPipeline;
        SDL_BindGPUGraphicsPipeline(renderPass, currentPipeline);
        Uint32 firstIndex = 0;
        for (const RenderCommand &command: commands) {
            SDL_GPUGraphicsPipeline *commandPipeline = command.pipeline != nullptr ? command.pipeline : defaultPipeline;
            if (commandPipeline != currentPipeline) {
                SDL_BindGPUGraphicsPipeline(renderPass, commandPipeline);
                currentPipeline = commandPipeline;
            }
            SDL_GPUTexture *texture = solidColorTexture_;
            if (command.texture != nullptr) {
                auto commandTexture = command.texture->GetResource();
                if (commandTexture != nullptr) {
                    texture = commandTexture;
                }
            }
            if (texture == nullptr) {
                firstIndex += 6;
                continue;
            }
            SDL_GPUSampler *sampler = defaultSampler;
            if (command.sampler != nullptr) {
                sampler = command.sampler;
            }
            SDL_GPUTextureSamplerBinding textureSamplerBinding = {texture, sampler};
            SDL_BindGPUFragmentSamplers(renderPass, 0, &textureSamplerBinding, 1);
            if (command.uniformBlock != nullptr) {
                command.uniformBlock->Fill(*injectContext, sceneStagingBuffer_);
                SDL_PushGPUFragmentUniformData(commandBuffer, command.uniformBlock->GetBinding(),
                                               sceneStagingBuffer_.data(), sceneStagingBuffer_.size());
            }
            SDL_DrawGPUIndexedPrimitives(renderPass, 6, 1, firstIndex, 0, 0);
            firstIndex += 6;
        }
    }

    SDL_EndGPURenderPass(renderPass);
}
