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
#include "GpuPipelineCache.h"

#include "core/gpu/BlendMode.h"
#include "core/gpu/SpriteVertex.h"
#include "core/utils/TomlUtils.h"
#include "toml11/parser.hpp"


SDL_GPUColorTargetBlendState glimmer::GpuPipelineCache::ToColorTargetBlendState(const BlendMode mode) {
    SDL_GPUColorTargetBlendState state = {};
    state.enable_blend = false;
    state.enable_color_write_mask = false;
    state.color_write_mask = 0;

    switch (mode) {
        case BlendMode::Opaque:
            break;
        case BlendMode::Alpha:
            state.enable_blend = true;
            state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
            state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
            state.color_blend_op = SDL_GPU_BLENDOP_ADD;
            state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
            state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
            state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
            break;
        case BlendMode::Additive:
            state.enable_blend = true;
            state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
            state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
            state.color_blend_op = SDL_GPU_BLENDOP_ADD;
            state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
            state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
            state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
            break;
        case BlendMode::Multiply:
            state.enable_blend = true;
            state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_DST_COLOR;
            state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
            state.color_blend_op = SDL_GPU_BLENDOP_ADD;
            state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
            state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
            state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
            break;
        case BlendMode::Premultiplied:
            state.enable_blend = true;
            state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
            state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
            state.color_blend_op = SDL_GPU_BLENDOP_ADD;
            state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
            state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
            state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
            break;
    }
    return state;
}

std::shared_ptr<glimmer::GPUPipelineResourceResult> glimmer::GpuPipelineCache::LoadResourceFromPack(
    AppContext *appContext, const ResourceRef *resourceRef, const ResourcePack *resourcePack) {
    std::filesystem::path pipelinePath = resourcePack->GetPath() / "pipelines" / resourceRef->GetPackageId() /
                                         resourceRef->GetResourceKey();
    pipelinePath.replace_extension("pipeline.toml");
    VirtualFileSystem *virtualFileSystem = appContext->GetVirtualFileSystem();
    if (virtualFileSystem == nullptr) {
        return nullptr;
    }
    if (!virtualFileSystem->Exists(pipelinePath)) {
        return nullptr;
    }
    auto data = virtualFileSystem->ReadFileAsString(pipelinePath);
    if (!data.has_value()) {
        return nullptr;
    }
    const WindowContext *windowContext = appContext->GetWindowContext();
    if (windowContext == nullptr) {
        return nullptr;
    }
    SDL_Window *window = windowContext->GetWindow();
    if (window == nullptr) {
        return nullptr;
    }
    SDL_GPUDevice *device = windowContext->GetDevice();
    if (device == nullptr) {
        return nullptr;
    }
    ResourceLocator *resourceLocator = appContext->GetResourceLocator();
    if (resourceLocator == nullptr) {
        return nullptr;
    }
    const std::string &manifestId = resourcePack->GetManifest()->id;
    auto gpuPipelineResource = std::make_unique<GPUPipelineResource>(
        toml::get<GPUPipelineResource>(toml::parse_str(data.value(), TOML_VERSION)));
    gpuPipelineResource->vertexShader.SetSelfPackageId(manifestId);
    gpuPipelineResource->fragmentShader.SetSelfPackageId(manifestId);

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
    colorTarget.blend_state = ToColorTargetBlendState(static_cast<BlendMode>(gpuPipelineResource->blendMode));
    colorTarget.format = SDL_GetGPUSwapchainTextureFormat(device, window);

    SDL_GPUGraphicsPipelineCreateInfo createInfo = {};
    createInfo.primitive_type = static_cast<SDL_GPUPrimitiveType>(gpuPipelineResource->primitiveType);
    createInfo.vertex_input_state.vertex_buffer_descriptions = &bufferDescription;
    createInfo.vertex_input_state.num_vertex_buffers = 1;
    createInfo.vertex_input_state.vertex_attributes = attributes;
    createInfo.vertex_input_state.num_vertex_attributes = 3;
    createInfo.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
    createInfo.target_info.color_target_descriptions = &colorTarget;
    createInfo.target_info.num_color_targets = 1;
    createInfo.target_info.has_depth_stencil_target = false;
    vertexShaderResult_ = resourceLocator->FindShader(
        &gpuPipelineResource->vertexShader);
    if (vertexShaderResult_ == nullptr) {
        return nullptr;
    }
    createInfo.vertex_shader = vertexShaderResult_->GetResource();
    fragmentShaderResult_ = resourceLocator->FindShader(
        &gpuPipelineResource->fragmentShader);
    if (fragmentShaderResult_ == nullptr) {
        return nullptr;
    }
    createInfo.fragment_shader = fragmentShaderResult_->GetResource();
    auto pipelineResourceResult = std::make_shared<GPUPipelineResourceResult>();
    pipelineResourceResult->SetResource(SDL_CreateGPUGraphicsPipeline(device, &createInfo));
    pipelineResourceResult->SetResourcePack(resourcePack);
    pipelineResourceResult->SetDevice(device);
    return pipelineResourceResult;
}


glimmer::GpuPipelineCache::~GpuPipelineCache() noexcept = default;
