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
#include <cstdint>
#include <unordered_map>

#include "core/mod/Resource.h"

namespace glimmer {
    class AppContext;

    /**
     * GpuPipelineObjectCache
     * GPU 管线对象缓存
     *
     * Caches SDL_GPUGraphicsPipeline objects created from GPUPipelineResource
     * configurations. Custom pipelines reuse the engine's SpriteVertex layout.
     * 根据 GPUPipelineResource 配置创建并缓存 SDL_GPUGraphicsPipeline。
     * 自定义管线复用引擎的 SpriteVertex 布局。
     */
    class GpuPipelineObjectCache {
        AppContext *appContext_ = nullptr;

        struct PipelineKey {
            uint64_t vertexShaderFingerprint = 0;
            uint64_t fragmentShaderFingerprint = 0;
            uint8_t blendMode = 0;

            [[nodiscard]] bool operator==(const PipelineKey &other) const {
                return vertexShaderFingerprint == other.vertexShaderFingerprint
                       && fragmentShaderFingerprint == other.fragmentShaderFingerprint
                       && blendMode == other.blendMode;
            }
        };

        struct PipelineKeyHash {
            [[nodiscard]] std::size_t operator()(const PipelineKey &key) const noexcept {
                // FNV-1a like combination
                std::size_t hash = 1469598103934665603ULL;
                hash ^= static_cast<std::size_t>(key.vertexShaderFingerprint);
                hash *= 1099511628211ULL;
                hash ^= static_cast<std::size_t>(key.fragmentShaderFingerprint);
                hash *= 1099511628211ULL;
                hash ^= static_cast<std::size_t>(key.blendMode);
                hash *= 1099511628211ULL;
                return hash;
            }
        };

        std::unordered_map<PipelineKey, SDL_GPUGraphicsPipeline *, PipelineKeyHash> pipelines_;

    public:
        explicit GpuPipelineObjectCache(AppContext *appContext);

        ~GpuPipelineObjectCache();

        /**
         * Get or create an SDL_GPUGraphicsPipeline for the given pipeline resource.
         * 获取或创建指定管线配置对应的 SDL_GPUGraphicsPipeline。
         * @param pipelineResource pipelineResource 管线配置
         * @return Pipeline handle on success, nullptr on failure.
         * 成功返回管线句柄，失败返回 nullptr。
         */
        [[nodiscard]] SDL_GPUGraphicsPipeline *GetOrCreatePipeline(
            const GPUPipelineResource *pipelineResource);

        /**
         * Clear all cached pipelines.
         * 清空所有缓存的管线。
         */
        void Clear();
    };
}
