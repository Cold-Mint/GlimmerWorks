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

#include <cstdint>
#include <memory>
#include <vector>

#include <SDL3/SDL_gpu.h>

#include "core/gpu/IPass.h"
#include "core/gpu/SpriteVertex.h"

namespace glimmer {
    class GPUPipelineResourceResult;
    class GPUSamplerResourceResult;

    /**
     * ScenePass
     * 场景 pass
     *
     * Uploads the sorted render queue into GPU buffers and draws it into the
     * offscreen scene texture (unlit). The produced texture is published on
     * RenderFrameContext::sceneTexture for the lighting pass to consume.
     * 把排好序的渲染队列上传到 GPU 缓冲，并绘制到离屏场景纹理（无光照）。
     * 产出的纹理会发布到 RenderFrameContext::sceneTexture 供光照 pass 消费。
     */
    class ScenePass final : public IPass {
        SDL_GPUDevice *device_ = nullptr;
        SDL_Window *window_ = nullptr;
        std::shared_ptr<GPUPipelineResourceResult> defaultPipeline_ = nullptr;
        std::shared_ptr<GPUSamplerResourceResult> defaultSampler_ = nullptr;

        //Offscreen render target for the unlit scene pass.
        //用于无光照场景通道的离屏渲染目标。
        SDL_GPUTexture *sceneTexture_ = nullptr;
        Uint32 sceneTextureWidth_ = 0;
        Uint32 sceneTextureHeight_ = 0;

        //1x1 white texture bound when a command has no texture, so the
        //fragment shader's `texture * color` resolves to just the vertex color
        //for solid-color geometry (rectangles/lines/points).
        //当命令没有纹理时绑定的 1x1 白色纹理，使片元着色器的
        //`texture * color` 退化为纯顶点颜色（矩形/线/点）。
        SDL_GPUTexture *solidColorTexture_ = nullptr;

        SDL_GPUBuffer *vertexBuffer_ = nullptr;
        SDL_GPUBuffer *indexBuffer_ = nullptr;
        Uint32 vertexBufferSize_ = 0;
        Uint32 indexBufferSize_ = 0;
        SDL_GPUTransferBuffer *transferBuffer_ = nullptr;
        Uint32 transferBufferSize_ = 0;

        //Per-frame staging buffer for scene-pass command uniform blocks.
        //场景 pass 命令 uniform 块的逐帧 staging 缓冲区。
        std::vector<uint8_t> sceneStagingBuffer_;
        //CPU-side staging buffers reused across frames to assemble the vertex
        //and index data before upload, avoiding per-frame heap allocation.
        //跨帧复用的 CPU 端暂存缓冲区，用于在上传前组装顶点/索引数据，
        //避免每帧堆分配。
        std::vector<SpriteVertex> vertexStaging_;
        std::vector<Uint32> indexStaging_;

        void EnsureSceneTexture(RenderFrameContext &ctx);

        void EnsureVertexBufferSize(Uint32 size);

        void EnsureIndexBufferSize(Uint32 size);

        void EnsureTransferBufferSize(Uint32 size);

        void EnsureSolidColorTexture();

        void FlushScenePass(RenderFrameContext &ctx);

    public:
        ScenePass(SDL_GPUDevice *device, SDL_Window *window,
                  std::shared_ptr<GPUPipelineResourceResult> defaultPipeline,
                  std::shared_ptr<GPUSamplerResourceResult> defaultSampler);

        ~ScenePass() override;

        void Prepare(RenderFrameContext &ctx) override;

        void Record(RenderFrameContext &ctx) override;
    };
}
