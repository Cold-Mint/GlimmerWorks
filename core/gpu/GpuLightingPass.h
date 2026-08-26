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
#include <vector>

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_stdinc.h>

#include "LightMapParams.h"

namespace glimmer {
    class GpuContext;
    class GpuTexture;

    /**
     * GpuLightingPass
     * GPU 光照通道
     *
     * Manages the per-tile light map texture and the full-screen lighting
     * quad draw. The light map is uploaded once per frame when the pixel data
     * changes, and sampled with bilinear filtering during the lighting pass.
     * 管理逐瓦片光照贴图纹理和全屏光照四边形绘制。像素数据变化时本帧上传
     * 一次光照贴图，并在光照通道中用双线性过滤采样。
     */
    class GpuLightingPass {
        std::unique_ptr<GpuTexture> lightMapTexture_;
        std::vector<Uint8> lightMapPixels_;
        int lightMapWidth_ = 0;
        int lightMapHeight_ = 0;
        LightMapParams lightMapParams_{};
        bool lightMapPending_ = false;
        bool lightMapDirty_ = false;

    public:
        GpuLightingPass() = default;

        /**
         * Provide this frame's light map data and parameters.
         * 提供本帧的光照贴图数据与参数。
         * @param rgbaPixels nullptr to reuse the previous GPU texture contents.
         * nullptr 表示复用上一帧的 GPU 纹理内容。
         */
        void SetLightMap(int width, int height, const Uint8 *rgbaPixels, const LightMapParams &params);

        /**
         * Consume the pending flag. Should be called once per frame.
         * 消费本帧的请求标记。每帧调用一次。
         * @return true if a light map was requested this frame.
         * 本帧有光照贴图请求时返回 true。
         */
        bool ConsumeRequest();

        /**
         * Create/recreate the GPU light map texture and upload the staging pixels.
         * Must be called while no render pass is active.
         * 创建/重建 GPU 光照贴图纹理并上传暂存像素。必须在无活动渲染通道时调用。
         * @return true if the GPU light map is valid and up to date.
         * GPU 光照贴图有效且为最新时返回 true。
         */
        bool Upload(SDL_GPUCommandBuffer *commandBuffer, GpuContext *context);

        /**
         * Draw the full-screen lighting quad using the given pipeline.
         * 使用给定管线绘制全屏光照四边形。
         */
        void Draw(SDL_GPUCommandBuffer *commandBuffer, SDL_GPURenderPass *renderPass,
                  SDL_GPUGraphicsPipeline *lightingPipeline, SDL_GPUBuffer *unitQuadBuffer,
                  SDL_GPUSampler *linearSampler);

        [[nodiscard]] GpuTexture *GetLightMapTexture() const { return lightMapTexture_.get(); }

        [[nodiscard]] const LightMapParams &GetLightMapParams() const { return lightMapParams_; }
    };
}
