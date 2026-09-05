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
#include "core/gpu/LightMapTexture.h"
#include "core/gpu/RenderQueue.h"
#include "core/mod/resourcePack/GPUSamplerResourceResult.h"

namespace glimmer {
    class ResourceLocator;
    class WorldContext;
    class CameraComponent;
    class Transform2DComponent;

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
        SDL_GPUBuffer *vertexBuffer_ = nullptr;
        SDL_GPUBuffer *indexBuffer_ = nullptr;
        Uint32 vertexBufferSize_ = 0;
        Uint32 indexBufferSize_ = 0;
        SDL_GPUTransferBuffer *transferBuffer_ = nullptr;
        Uint32 transferBufferSize_ = 0;
        std::shared_ptr<GPUPipelineResourceResult> lightingPipeline_ = nullptr;
        std::shared_ptr<GPUPipelineResourceResult> defaultPipeline_ = nullptr;
        std::shared_ptr<GPUSamplerResourceResult> defaultSampler_ = nullptr;
        std::shared_ptr<GPUSamplerResourceResult> lightingSampler_ = nullptr;

        //1x1 white texture bound when a command has no texture, so the
        //fragment shader's `texture * color` resolves to just the vertex color
        //for solid-color geometry (rectangles/lines/points).
        //当命令没有纹理时绑定的 1x1 白色纹理，使片元着色器的
        //`texture * color` 退化为纯顶点颜色（矩形/线/点）。
        SDL_GPUTexture *solidColorTexture_ = nullptr;

        //Offscreen render target for the unlit scene pass, plus the lighting
        //pipeline/sampler and per-tile light map texture used by the lighting pass.
        //用于无光照场景通道的离屏渲染目标，以及光照通道使用的光照管线/采样器与逐瓦片光照贴图纹理。
        SDL_GPUTexture *sceneTexture_ = nullptr;
        Uint32 sceneTextureWidth_ = 0;
        Uint32 sceneTextureHeight_ = 0;
        LightMapTexture lightMapTexture_;
        //Lighting shader uniform parameters (4 x vec4, std140 friendly).
        //光照着色器 uniform 参数（4 x vec4，std140 友好）。
        float lightingParams_[16] = {};

        void RenderScenes();

        void RenderOverlays();


        /**
         * Create or resize the offscreen scene render target.
         * 创建或调整离屏场景渲染目标。
         */
        void EnsureSceneTexture(Uint32 width, Uint32 height);

        /**
         * Create the 1x1 white texture used to render solid-color geometry
         * (rectangles, lines and points submitted with a null texture).
         * 创建用于渲染纯色几何图形（以空纹理提交的矩形、线与点）的 1x1 白色纹理。
         */
        void EnsureSolidColorTexture();

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
         * Upload the queued commands to the GPU and draw them into the given
         * render target within a single clear-and-store render pass.
         * 把排队的命令上传到 GPU，并在一次 clear/store 渲染通道内绘制到指定渲染目标。
         */
        void FlushScenePass(SDL_GPUCommandBuffer *commandBuffer, SDL_GPUTexture *targetTexture, Uint32 width,
                            Uint32 height);

        /**
         * Composite the lit result: a fullscreen quad samples the unlit scene
         * texture and the light map, applies lighting, and stores to the target.
         * 合成受光照结果：全屏四边形采样无光照场景纹理与光照贴图，应用光照后写入目标。
         */
        void FlushLightingPass(SDL_GPUCommandBuffer *commandBuffer, SDL_GPUTexture *targetTexture);

        /**
         * Rebuild the per-tile light map texture from the light buffer for the
         * camera viewport (plus a one-tile border).
         * 根据相机视口（含一格边距）从光照缓冲重建逐瓦片光照贴图纹理。
         */
        void UpdateLightMap(const LightBuffer *lightBuffer, const CameraComponent *camera,
                            const Transform2DComponent *cameraTransform, const WorldContext *worldContext,
                            Uint32 width, Uint32 height);

    public:
        explicit AppRenderer(AppContext *appContext);

        /**
         * RenderFrame
         * 渲染一帧。
         * @param rmlContext
         * @param windowWidth
         * @param windowHeight
         */
        void RenderFrame(const RmlContext *rmlContext, int windowWidth, int windowHeight);
    };
}
