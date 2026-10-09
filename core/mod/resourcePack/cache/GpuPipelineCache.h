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
#include "core/gpu/BlendMode.h"
#include "core/mod/resourcePack/BaseResourceCache.h"

namespace glimmer {
    class GpuPipelineCache : public BaseResourceCache<GPUPipelineResourceResult> {
        std::shared_ptr<ShaderResourceResult> vertexShaderResult_ = nullptr;
        std::shared_ptr<ShaderResourceResult> fragmentShaderResult_ = nullptr;


        /**
        * Convert a BlendMode into an SDL3 GPU color target blend state.
        * 将 BlendMode 转换为 SDL3 GPU 颜色目标混合状态。
        * @param mode mode 混合模式
        */
        static SDL_GPUColorTargetBlendState ToColorTargetBlendState(BlendMode mode);

    protected:
        std::shared_ptr<GPUPipelineResourceResult> LoadResourceFromPack(AppContext *appContext,
                                                                        const ResourceRef *resourceRef,
                                                                        const ResourcePack *resourcePack) override;

    public:
        ~GpuPipelineCache() noexcept override;
    };
}
