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
#include <SDL3/SDL_pixels.h>

#include "GpuContext.h"
#include "GpuTexture.h"
#include "LightMapParams.h"

namespace glimmer {
    class AppContext;
    class RenderQueue;
    class GpuCompositor;
    class GpuFrameState;
    class GpuLayerTextures;
    class GpuLightingPass;
    class GpuRenderPassState;
    class GpuShaderPipelineManager;
    class GpuSpriteBatcher;

    /**
     * GpuRenderer
     * GPU 渲染器
     *
     * Frame-level renderer built on SDL_GPU. This class orchestrates the
     * rendering pipeline but delegates the actual work to smaller, single-
     * responsibility helpers:
     *   - GpuFrameState: command buffer / swapchain management
     *   - GpuLayerTextures: offscreen game/ui/composite textures
     *   - GpuRenderPassState: active render pass tracking
     *   - GpuShaderPipelineManager: shader / pipeline cache
     *   - GpuSpriteBatcher: dynamic vertex buffer and command batching
     *   - GpuLightingPass: light map texture and lighting quad
     *   - GpuCompositor: full-screen quads and layered composite passes
     * 基于 SDL_GPU 的帧级渲染器。本类负责协调渲染管线，但把实际工作委托给
     * 更小的单一职责辅助类：
     *   - GpuFrameState：命令缓冲 / 交换链管理
     *   - GpuLayerTextures：离屏 game/ui/composite 纹理
     *   - GpuRenderPassState：活动渲染通道跟踪
     *   - GpuShaderPipelineManager：着色器 / 管线缓存
     *   - GpuSpriteBatcher：动态顶点缓冲与命令合批
     *   - GpuLightingPass：光照贴图纹理与光照四边形
     *   - GpuCompositor：全屏四边形与分层合成通道
     */
    class GpuRenderer {
        std::unique_ptr<GpuFrameState> frameState_;
        std::unique_ptr<GpuRenderPassState> renderPassState_;
        std::unique_ptr<GpuLayerTextures> layerTextures_;
        std::unique_ptr<GpuShaderPipelineManager> pipelineManager_;
        std::unique_ptr<GpuSpriteBatcher> spriteBatcher_;
        std::unique_ptr<GpuLightingPass> lightingPass_;
        std::unique_ptr<GpuCompositor> compositor_;
        GpuContext *gpuContext_ = nullptr;
        SDL_Color clearColor_ = {0, 0, 0, 255};

    public:
        GpuRenderer();

        ~GpuRenderer();

        GpuRenderer(const GpuRenderer &) = delete;

        GpuRenderer &operator=(const GpuRenderer &) = delete;

        /**
         * Initialize the renderer and all its sub-components.
         * 初始化渲染器及其所有子组件。
         */
        bool Init(GpuContext *gpuContext, AppContext *appContext);

        /**
         * Release all GPU resources.
         * 释放所有 GPU 资源。
         */
        void Shutdown();

        /**
         * Begin a new frame.
         * 开始新帧。
         */
        bool BeginFrame(SDL_Window *window);

        /**
         * Sort, upload and draw the render queue into the game layer.
         * 对渲染队列排序、上传并绘制进 game 层。
         */
        void FlushQueue(RenderQueue &queue);

        /**
         * Provide this frame's light map.
         * 提供本帧的光照贴图。
         */
        void SetLightMap(int width, int height, const Uint8 *rgbaPixels, const LightMapParams &params);

        /**
         * @return The offscreen ui layer texture.
         * 离屏 ui 层纹理。
         */
        [[nodiscard]] GpuTexture *GetUiTargetTexture() const;

        /**
         * Composite the game/ui layers onto the swapchain.
         * 将 game/ui 层合成到交换链。
         */
        void CompositeToSwapchain();

        /**
         * Submit the frame command buffer.
         * 提交帧命令缓冲。
         */
        bool SubmitFrame();

        [[nodiscard]] SDL_GPUCommandBuffer *GetCommandBuffer() const;

        [[nodiscard]] SDL_GPUTexture *GetSwapchainTexture() const;

        [[nodiscard]] Uint32 GetSwapchainWidth() const;

        [[nodiscard]] Uint32 GetSwapchainHeight() const;

        void SetClearColor(SDL_Color color);

        [[nodiscard]] SDL_Color GetClearColor() const;
    };
}
