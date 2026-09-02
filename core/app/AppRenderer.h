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
#include <string>

#include <SDL3/SDL_gpu.h>

#include "core/context/AppContext.h"
#include "core/gpu/RenderQueue.h"

namespace glimmer {
    class ResourceLocator;

    /**
     * AppRenderer
     * 应用渲染器
     *
     * Drives one frame of rendering: clears the per-frame RenderQueue, lets
     * the scenes/overlays/UI messages submit their commands, then uploads the
     * sorted queue into GPU buffers and draws it into the swapchain. RmlUi is
     * composited on top afterwards, then the frame is submitted.
     * 驱动一帧的渲染：清空每帧的 RenderQueue，让场景/覆盖层/UI 消息提交
     * 命令，然后把排好序的队列上传进 GPU 缓冲并绘制到交换链，随后把 RmlUi
     * 合成在上层，最后提交这一帧。
     */
    class AppRenderer {
        AppContext *appContext_ = nullptr;
        RenderQueue renderQueue_;

        SDL_GPUDevice *device_ = nullptr;
        ResourceLocator *resourceLocator_ = nullptr;

        SDL_GPUGraphicsPipeline *spritePipeline_ = nullptr;
        SDL_GPUBuffer *vertexBuffer_ = nullptr;
        SDL_GPUBuffer *indexBuffer_ = nullptr;
        Uint32 vertexBufferSize_ = 0;
        Uint32 indexBufferSize_ = 0;
        SDL_GPUTransferBuffer *transferBuffer_ = nullptr;
        Uint32 transferBufferSize_ = 0;
        SDL_GPUTexture *whiteTexture_ = nullptr;
        SDL_GPUSampler *sampler_ = nullptr;

        void RenderScenes();

        void RenderOverlays();

        /**
         * Create the shared GPU resources (white texture and sampler) if they
         * have not been created yet.
         * 若尚未创建，创建共享 GPU 资源（白色纹理与采样器）。
         */
        bool EnsureGpuResources();

        /**
         * Create the sprite graphics pipeline on demand. Tries to load the
         * pipeline from the resource pack first; falls back to the built-in
         * sprite shaders when the resource pack is missing.
         * 按需创建精灵图形管线。优先从资源包加载管线；资源包缺失时回退到
         * 内置精灵着色器。
         */
        bool EnsureSpritePipeline();

        /**
         * Compile a GLSL source to SPIR-V and wrap it in an SDL_GPUShader.
         * The caller owns the returned shader.
         * 将 GLSL 源码编译为 SPIR-V 并封装为 SDL_GPUShader。调用方持有返回
         * 着色器的所有权。
         */
        [[nodiscard]] SDL_GPUShader *CompileShader(const std::string &source, bool vertex) const;

        /**
         * Grow the vertex buffer to at least `size` bytes if necessary.
         * 必要时把顶点缓冲扩容到至少 size 字节。
         */
        void EnsureVertexBufferSize(Uint32 size);

        /**
         * Grow the index buffer to at least `size` bytes if necessary.
         * 必要时把索引缓冲扩容到至少 size 字节。
         */
        void EnsureIndexBufferSize(Uint32 size);

        /**
         * Grow the transfer buffer to at least `size` bytes if necessary.
         * 必要时把传输缓冲扩容到至少 size 字节。
         */
        void EnsureTransferBufferSize(Uint32 size);

        /**
         * Upload the queued commands to the GPU and draw them into the
         * swapchain within a single clear-and-store render pass.
         * 把排队的命令上传到 GPU，并在一次 clear/store 渲染通道内绘制到交换链。
         */
        void FlushQueue(SDL_GPUCommandBuffer *commandBuffer, SDL_GPUTexture *swapchainTexture, Uint32 width,
                        Uint32 height);

    public:
        explicit AppRenderer(AppContext *appContext);

        /**
         * RenderFrame
         * 渲染一帧。
         * @param rmlContext
         * @param windowWidth
         * @param windowHeight
         * @param frameStart
         * @param deltaTime
         */
        void RenderFrame(const RmlContext *rmlContext, int windowWidth, int windowHeight, uint64_t frameStart,
                         float deltaTime);
    };
}
