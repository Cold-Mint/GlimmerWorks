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

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

#include <SDL3/SDL_gpu.h>

#include "SpriteBlendMode.h"
#include "core/mod/resourcePack/GPUPipelineResource.h"

namespace glimmer {
    class ResourceLocator;
    class VirtualFileSystem;
    class GpuShaderCache;

    /**
     * GpuShaderPipelineManager
     * GPU 着色器与管线管理器
     *
     * Owns the in-memory shader/pipeline caches and the disk shader cache.
     * Creates pipelines lazily on first request. Pipeline configurations are
     * loaded from the core resource pack; only the sprite pipeline keeps a
     * hardcoded fallback (one vertex + one fragment shader).
     * 持有内存中的着色器/管线缓存和磁盘着色器缓存。首次请求时惰性创建管线。
     * 管线配置从 core 资源包加载；只有 sprite 管线保留硬编码回退
     * （1 个顶点着色器 + 1 个片元着色器）。
     */
    class GpuShaderPipelineManager {
        SDL_GPUDevice *device_ = nullptr;
        ResourceLocator *resourceLocator_ = nullptr;
        VirtualFileSystem *virtualFileSystem_ = nullptr;
        std::unique_ptr<GpuShaderCache> gpuShaderCache_;
        std::unordered_map<std::string, SDL_GPUShader *> shaderCache_;
        std::unordered_map<std::string, SDL_GPUGraphicsPipeline *> pipelineCache_;
        SDL_GPUTextureFormat colorFormat_ = SDL_GPU_TEXTUREFORMAT_INVALID;

        [[nodiscard]] SDL_GPUShader *GetOrCreateShader(const std::string &key, const std::string &extension,
                                                       SDL_GPUShaderStage stage, Uint32 numSamplers,
                                                       Uint32 numUniformBuffers, const char *fallbackSource);

        [[nodiscard]] SDL_GPUGraphicsPipeline *GetOrCreatePipeline(const std::string &name, bool allowFallback);

        [[nodiscard]] SDL_GPUGraphicsPipeline *CreateSpritePipeline(SDL_GPUShader *vertexShader,
                                                                    SDL_GPUShader *fragmentShader,
                                                                    SpriteBlendMode blendMode) const;

        [[nodiscard]] SDL_GPUGraphicsPipeline *CreatePipelineFromConfig(const GPUPipelineResource &config);

        [[nodiscard]] SDL_GPUGraphicsPipeline *CreateFallbackPipeline();

    public:
        GpuShaderPipelineManager();

        ~GpuShaderPipelineManager();

        bool Init(SDL_GPUDevice *device, ResourceLocator *locator, VirtualFileSystem *vfs,
                  const std::filesystem::path &cachePath, SDL_GPUTextureFormat colorFormat);

        void Shutdown();

        [[nodiscard]] SDL_GPUGraphicsPipeline *GetSpritePipeline();

        [[nodiscard]] SDL_GPUGraphicsPipeline *GetGamePipeline();

        [[nodiscard]] SDL_GPUGraphicsPipeline *GetUiPipeline();

        [[nodiscard]] SDL_GPUGraphicsPipeline *GetGlobalPipeline();

        [[nodiscard]] SDL_GPUGraphicsPipeline *GetLightingPipeline();
    };
}
