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
#include "GpuCompositor.h"

#include "GpuContext.h"
#include "GpuLayerTextures.h"
#include "GpuRenderPassState.h"
#include "GpuShaderPipelineManager.h"
#include "GpuTexture.h"
#include "SpriteVertex.h"
#include "core/log/LogCat.h"

namespace glimmer {
    bool GpuCompositor::Init(SDL_GPUDevice *device) {
        if (device == nullptr) {
            return false;
        }
        const SpriteVertex unitQuadVertices[6] = {
            {0.0F, 0.0F, 0.0F, 0.0F, 255, 255, 255, 255},
            {1.0F, 0.0F, 1.0F, 0.0F, 255, 255, 255, 255},
            {0.0F, 1.0F, 0.0F, 1.0F, 255, 255, 255, 255},
            {1.0F, 0.0F, 1.0F, 0.0F, 255, 255, 255, 255},
            {1.0F, 1.0F, 1.0F, 1.0F, 255, 255, 255, 255},
            {0.0F, 1.0F, 0.0F, 1.0F, 255, 255, 255, 255}
        };
        SDL_GPUBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
        bufferCreateInfo.size = sizeof(unitQuadVertices);
        bufferCreateInfo.props = 0;
        unitQuadBuffer_ = SDL_CreateGPUBuffer(device, &bufferCreateInfo);
        if (unitQuadBuffer_ == nullptr) {
            LogCat::e(std::source_location::current(), "SDL_CreateGPUBuffer(unit quad) failed: ", SDL_GetError());
            return false;
        }
        SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {};
        transferBufferCreateInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transferBufferCreateInfo.size = sizeof(unitQuadVertices);
        transferBufferCreateInfo.props = 0;
        SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferBufferCreateInfo);
        if (transferBuffer == nullptr) {
            LogCat::e(std::source_location::current(), "SDL_CreateGPUTransferBuffer failed: ", SDL_GetError());
            Shutdown(device);
            return false;
        }
        void *mapped = SDL_MapGPUTransferBuffer(device, transferBuffer, false);
        if (mapped == nullptr) {
            SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
            Shutdown(device);
            return false;
        }
        SDL_memcpy(mapped, unitQuadVertices, sizeof(unitQuadVertices));
        SDL_UnmapGPUTransferBuffer(device, transferBuffer);
        SDL_GPUCommandBuffer *uploadCommandBuffer = SDL_AcquireGPUCommandBuffer(device);
        SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(uploadCommandBuffer);
        SDL_GPUTransferBufferLocation location = {};
        location.transfer_buffer = transferBuffer;
        location.offset = 0;
        SDL_GPUBufferRegion region = {};
        region.buffer = unitQuadBuffer_;
        region.offset = 0;
        region.size = sizeof(unitQuadVertices);
        SDL_UploadToGPUBuffer(copyPass, &location, &region, false);
        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(uploadCommandBuffer);
        SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
        return true;
    }

    void GpuCompositor::Shutdown(SDL_GPUDevice *device) {
        if (unitQuadBuffer_ != nullptr && device != nullptr) {
            SDL_ReleaseGPUBuffer(device, unitQuadBuffer_);
            unitQuadBuffer_ = nullptr;
        }
    }

    void GpuCompositor::DrawLayerQuad(SDL_GPUCommandBuffer *commandBuffer, SDL_GPURenderPass *renderPass,
                                      SDL_GPUGraphicsPipeline *pipeline, const GpuTexture *source,
                                      const Uint32 viewportWidth, const Uint32 viewportHeight,
                                      SDL_GPUSampler *sampler) {
        if (commandBuffer == nullptr || renderPass == nullptr || pipeline == nullptr || source == nullptr ||
            !source->IsValid()) {
            return;
        }
        SDL_BindGPUGraphicsPipeline(renderPass, pipeline);
        SDL_GPUViewport viewport = {};
        viewport.x = 0.0F;
        viewport.y = 0.0F;
        viewport.w = static_cast<float>(viewportWidth);
        viewport.h = static_cast<float>(viewportHeight);
        viewport.min_depth = 0.0F;
        viewport.max_depth = 1.0F;
        SDL_SetGPUViewport(renderPass, &viewport);
        const float viewSize[2] = {1.0F, 1.0F};
        SDL_PushGPUVertexUniformData(commandBuffer, 0, viewSize, sizeof(viewSize));
        SDL_GPUBufferBinding vertexBufferBinding = {};
        vertexBufferBinding.buffer = unitQuadBuffer_;
        vertexBufferBinding.offset = 0;
        SDL_BindGPUVertexBuffers(renderPass, 0, &vertexBufferBinding, 1);
        SDL_GPUTextureSamplerBinding textureBinding = {};
        textureBinding.texture = source->GetGpuTexture();
        textureBinding.sampler = sampler;
        SDL_BindGPUFragmentSamplers(renderPass, 0, &textureBinding, 1);
        SDL_DrawGPUPrimitives(renderPass, 6, 1, 0, 0);
    }

    void GpuCompositor::Composite(SDL_GPUCommandBuffer *commandBuffer,
                                  GpuRenderPassState &passState,
                                  GpuShaderPipelineManager &pipelines,
                                  GpuLayerTextures &layers,
                                  SDL_GPUTexture *swapchainTexture,
                                  const Uint32 viewportWidth, const Uint32 viewportHeight,
                                  SDL_GPUSampler *sampler) {
        if (commandBuffer == nullptr || swapchainTexture == nullptr) {
            return;
        }
        passState.End();
        //Pass 1: composite = game + ui.
        passState.Begin(commandBuffer, layers.GetCompositeLayer()->GetGpuTexture(), true,
                        SDL_Color{0, 0, 0, 255});
        if (SDL_GPURenderPass *renderPass = passState.GetRenderPass(); renderPass != nullptr) {
            DrawLayerQuad(commandBuffer, renderPass, pipelines.GetGamePipeline(), layers.GetGameLayer(), viewportWidth,
                          viewportHeight, sampler);
            DrawLayerQuad(commandBuffer, renderPass, pipelines.GetUiPipeline(), layers.GetUiLayer(), viewportWidth,
                          viewportHeight, sampler);
        }
        passState.End();
        //Pass 2: swapchain = composite.
        passState.Begin(commandBuffer, swapchainTexture, true, SDL_Color{0, 0, 0, 255});
        if (SDL_GPURenderPass *renderPass = passState.GetRenderPass(); renderPass != nullptr) {
            DrawLayerQuad(commandBuffer, renderPass, pipelines.GetGlobalPipeline(), layers.GetCompositeLayer(),
                          viewportWidth, viewportHeight, sampler);
        }
        passState.End();
    }
}
