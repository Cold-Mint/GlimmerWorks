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

#include <SDL3/SDL_gpu.h>

namespace glimmer {
    class GpuContext;
    class GpuLayerTextures;
    class GpuRenderPassState;
    class GpuShaderPipelineManager;
    class GpuTexture;

    /**
     * GpuCompositor
     * GPU 合成器
     *
     * Manages the static full-screen unit quad buffer and performs the
     * layered composite passes: game + ui -> composite, then composite -> swapchain.
     * 管理静态全屏单位四边形缓冲，并执行分层合成通道：game + ui → composite，
     * 然后 composite → swapchain。
     */
    class GpuCompositor {
        SDL_GPUBuffer *unitQuadBuffer_ = nullptr;

    public:
        GpuCompositor() = default;

        /**
         * Create the static full-screen unit quad vertex buffer.
         * 创建静态全屏单位四边形顶点缓冲。
         */
        bool Init(SDL_GPUDevice *device);

        /**
         * Release the unit quad buffer.
         * 释放单位四边形缓冲。
         */
        void Shutdown(SDL_GPUDevice *device);

        /**
         * Draw a full-screen quad sampling the given source texture with the
         * given pipeline.
         * 使用给定管线和源纹理绘制一个全屏四边形。
         */
        void DrawLayerQuad(SDL_GPUCommandBuffer *commandBuffer, SDL_GPURenderPass *renderPass,
                           SDL_GPUGraphicsPipeline *pipeline, const GpuTexture *source,
                           Uint32 viewportWidth, Uint32 viewportHeight,
                           SDL_GPUSampler *sampler);

        /**
         * Perform both composite passes.
         * 执行两个合成通道。
         */
        void Composite(SDL_GPUCommandBuffer *commandBuffer,
                       GpuRenderPassState &passState,
                       GpuShaderPipelineManager &pipelines,
                       GpuLayerTextures &layers,
                       SDL_GPUTexture *swapchainTexture,
                       Uint32 viewportWidth, Uint32 viewportHeight,
                       SDL_GPUSampler *sampler);

        [[nodiscard]] SDL_GPUBuffer *GetUnitQuadBuffer() const { return unitQuadBuffer_; }
    };
}
