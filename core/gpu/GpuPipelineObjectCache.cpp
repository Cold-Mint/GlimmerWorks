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
#include "GpuPipelineObjectCache.h"

#include <source_location>

#include "core/context/AppContext.h"
#include "core/context/WindowContext.h"
#include "core/gpu/BlendMode.h"
#include "core/gpu/SpriteVertex.h"
#include "core/log/LogCat.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/resourcePack/GPUPipelineResource.h"
#include "core/mod/resourcePack/ShaderResourceResult.h"

namespace glimmer {
    static SDL_GPUGraphicsPipeline *CreateSpritePipeline(
        AppContext *appContext,
        SDL_GPUShader *vertexShader,
        SDL_GPUShader *fragmentShader,
        BlendMode blendMode) {
        if (appContext == nullptr || vertexShader == nullptr || fragmentShader == nullptr) {
            return nullptr;
        }
        WindowContext *windowContext = appContext->GetWindowContext();
        if (windowContext == nullptr) {
            return nullptr;
        }
        SDL_GPUDevice *device = windowContext->GetDevice();
        if (device == nullptr) {
            return nullptr;
        }
        SDL_Window *window = windowContext->GetWindow();
        if (window == nullptr) {
            return nullptr;
        }

        SDL_GPUVertexBufferDescription bufferDescription = {};
        bufferDescription.slot = 0;
        bufferDescription.pitch = sizeof(SpriteVertex);
        bufferDescription.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;

        SDL_GPUVertexAttribute attributes[3] = {};
        attributes[0].location = 0;
        attributes[0].buffer_slot = 0;
        attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
        attributes[0].offset = offsetof(SpriteVertex, x);
        attributes[1].location = 1;
        attributes[1].buffer_slot = 0;
        attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
        attributes[1].offset = offsetof(SpriteVertex, u);
        attributes[2].location = 2;
        attributes[2].buffer_slot = 0;
        attributes[2].format = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM;
        attributes[2].offset = offsetof(SpriteVertex, r);

        SDL_GPUColorTargetDescription colorTarget = {};
        colorTarget.format = SDL_GetGPUSwapchainTextureFormat(device, window);
        colorTarget.blend_state = ToColorTargetBlendState(blendMode);

        SDL_GPUGraphicsPipelineCreateInfo createInfo = {};
        createInfo.vertex_shader = vertexShader;
        createInfo.fragment_shader = fragmentShader;
        createInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
        createInfo.vertex_input_state.vertex_buffer_descriptions = &bufferDescription;
        createInfo.vertex_input_state.num_vertex_buffers = 1;
        createInfo.vertex_input_state.vertex_attributes = attributes;
        createInfo.vertex_input_state.num_vertex_attributes = 3;
        createInfo.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
        createInfo.target_info.color_target_descriptions = &colorTarget;
        createInfo.target_info.num_color_targets = 1;
        createInfo.target_info.has_depth_stencil_target = false;

        SDL_GPUGraphicsPipeline *pipeline = SDL_CreateGPUGraphicsPipeline(device, &createInfo);
        if (pipeline == nullptr) {
            LogCat::w(std::source_location::current(), "SDL_CreateGPUGraphicsPipeline failed: ", SDL_GetError());
        }
        return pipeline;
    }
}

glimmer::GpuPipelineObjectCache::GpuPipelineObjectCache(AppContext *appContext) : appContext_(appContext) {
}

glimmer::GpuPipelineObjectCache::~GpuPipelineObjectCache() {
    Clear();
}

SDL_GPUGraphicsPipeline *glimmer::GpuPipelineObjectCache::GetOrCreatePipeline(
    const GPUPipelineResource *pipelineResource) {
    if (pipelineResource == nullptr || appContext_ == nullptr) {
        return nullptr;
    }
    const PipelineKey key{
        pipelineResource->vertexShader.GetFingerprint(),
        pipelineResource->fragmentShader.GetFingerprint(),
        pipelineResource->blendMode
    };
    const auto it = pipelines_.find(key);
    if (it != pipelines_.end()) {
        return it->second;
    }

    ResourceLocator *resourceLocator = appContext_->GetResourceLocator();
    if (resourceLocator == nullptr) {
        return nullptr;
    }

    const std::shared_ptr<ShaderResourceResult> vertexResult = resourceLocator->FindShader(
        &pipelineResource->vertexShader, false);
    const std::shared_ptr<ShaderResourceResult> fragmentResult = resourceLocator->FindShader(
        &pipelineResource->fragmentShader, false);
    if (vertexResult == nullptr || fragmentResult == nullptr) {
        return nullptr;
    }

    SDL_GPUShader *vertexShader = vertexResult->GetResource();
    SDL_GPUShader *fragmentShader = fragmentResult->GetResource();
    if (vertexShader == nullptr || fragmentShader == nullptr) {
        return nullptr;
    }

    SDL_GPUGraphicsPipeline *pipeline = CreateSpritePipeline(
        appContext_, vertexShader, fragmentShader,
        BlendModeFromUint8(pipelineResource->blendMode));
    if (pipeline == nullptr) {
        return nullptr;
    }
    pipelines_[key] = pipeline;
    return pipeline;
}

void glimmer::GpuPipelineObjectCache::Clear() {
    if (appContext_ == nullptr) {
        pipelines_.clear();
        return;
    }
    WindowContext *windowContext = appContext_->GetWindowContext();
    SDL_GPUDevice *device = windowContext != nullptr ? windowContext->GetDevice() : nullptr;
    for (const auto &[key, pipeline]: pipelines_) {
        if (device != nullptr && pipeline != nullptr) {
            SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
        }
    }
    pipelines_.clear();
}
