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

#include "core/gpu/IPass.h"
#include "core/gpu/LightMapTexture.h"
#include "core/math/Color.h"

namespace glimmer {
    class ResourceLocator;
    class UniformInjectContext;
    class GPUPipelineResourceResult;
    class GPUSamplerResourceResult;

    /**
     * LightingPass
     * 光照合成 pass
     *
     * Rebuilds the per-tile light map (CPU-side, in Prepare), uploads it and
     * composites the lit result: a fullscreen triangle samples the unlit scene
     * texture and the light map, applies lighting, and stores to the swapchain.
     * 重建逐瓦片光照贴图（CPU 端，位于 Prepare），上传它并合成受光照结果：
     * 全屏三角形采样无光照场景纹理与光照贴图，应用光照后写入交换链。
     */
    class LightingPass final : public IPass {
        ResourceLocator *resourceLocator_ = nullptr;
        SDL_GPUDevice *device_ = nullptr;
        std::shared_ptr<GPUPipelineResourceResult> lightingPipeline_ = nullptr;
        std::shared_ptr<GPUSamplerResourceResult> lightingSampler_ = nullptr;
        //Per-tile light map texture used by the lighting pass.
        //光照 pass 使用的逐瓦片光照贴图纹理。
        LightMapTexture lightMapTexture_;
        //Cached ambient light resolved from the dimension's keyframes at the
        //fixed initial time (no day/night flow yet), so color resources are
        //not re-resolved every frame.
        //按固定初始时间从维度关键帧解析并缓存的环境光（暂无昼夜流动），
        //避免每帧重复解析颜色资源。
        Color ambientLight_;
        //Per-frame staging buffer for the lighting uniform block; static
        //members are restored from the compiled block and dynamic members are
        //injected each frame before being pushed to the GPU.
        //光照 uniform 块的逐帧 staging 缓冲区；静态成员从编译块恢复，
        //动态成员每帧注入后再推送至 GPU。
        std::vector<uint8_t> lightingStagingBuffer_;

        void UpdateLightMap(UniformInjectContext *injectContext);

        void FlushLightingPass(RenderFrameContext &ctx);

    public:
        LightingPass(ResourceLocator *resourceLocator, SDL_GPUDevice *device,
                     std::shared_ptr<GPUPipelineResourceResult> lightingPipeline,
                     std::shared_ptr<GPUSamplerResourceResult> lightingSampler);

        void Prepare(RenderFrameContext &ctx) override;

        void Record(RenderFrameContext &ctx) override;
    };
}
