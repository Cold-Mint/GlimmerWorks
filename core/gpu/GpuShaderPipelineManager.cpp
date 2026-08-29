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
#include "GpuShaderPipelineManager.h"

#include "GpuShaderCache.h"
#include "GpuShaderCompiler.h"
#include "SpriteVertex.h"
#include "core/config/Constants.h"
#include "core/log/LogCat.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/ResourceRef.h"
#include "core/mod/resourcePack/GPUPipelineResource.h"
#include "core/mod/resourcePack/ShaderResourceResult.h"


SDL_GPUShader *glimmer::GpuShaderPipelineManager::GetOrCreateShader(const std::string &key,
                                                                    const std::string &extension,
                                                                    SDL_GPUShaderStage stage, Uint32 numSamplers,
                                                                    Uint32 numUniformBuffers,
                                                                    const char *fallbackSource) {
    const std::string shaderKey = key + "." + extension;
    if (const auto it = shaderCache_.find(shaderKey); it != shaderCache_.end()) {
        return it->second;
    }
    if (resourceLocator_ != nullptr) {
        ResourceRef resourceRef;
        resourceRef.SetPackageId(RESOURCE_REF_CORE);
        resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
        resourceRef.SetResourceType(RESOURCE_SHADER);
        resourceRef.SetResourceKey(shaderKey);
        if (auto shaderResult = resourceLocator_->FindShader(&resourceRef); shaderResult != nullptr) {
            SDL_GPUShader *shader = nullptr;
            if (gpuShaderCache_ != nullptr) {
                if (auto spirv = gpuShaderCache_->TryLoad(&resourceRef, shaderResult->GetPath(),
                                                          shaderResult->GetSource());
                    spirv.has_value()) {
                    shader = GpuShaderCompiler::CreateFromSpirv(device_, spirv->data(), spirv->size(),
                                                                shaderKey.c_str(), stage, numSamplers,
                                                                numUniformBuffers);
                }
            }
            if (shader == nullptr) {
                const std::string debugName = shaderResult->GetPath().string();
                if (const auto spirv = GpuShaderCompiler::CompileToSpirv(shaderResult->GetSource().c_str(),
                                                                         debugName.c_str(), stage);
                    !spirv.empty()) {
                    if (gpuShaderCache_ != nullptr) {
                        gpuShaderCache_->Store(&resourceRef, shaderResult->GetPath(), shaderResult->GetSource(),
                                               spirv.data(), spirv.size() * sizeof(unsigned int));
                    }
                    shader = GpuShaderCompiler::CreateFromSpirv(device_, spirv.data(),
                                                                spirv.size() * sizeof(unsigned int),
                                                                shaderKey.c_str(), stage, numSamplers,
                                                                numUniformBuffers);
                }
            }
            if (shader != nullptr) {
                shaderCache_[shaderKey] = shader;
                return shader;
            }
            LogCat::w(std::source_location::current(), "Pack shader failed to compile, using fallback: ",
                      shaderKey);
        }
    }
    SDL_GPUShader *fallbackShader = GpuShaderCompiler::CompileFromSource(
        device_, fallbackSource, (shaderKey + "(fallback)").c_str(), stage, numSamplers, numUniformBuffers);
    if (fallbackShader != nullptr) {
        shaderCache_[shaderKey] = fallbackShader;
    }
    return fallbackShader;
}

SDL_GPUGraphicsPipeline *glimmer::GpuShaderPipelineManager::GetOrCreatePipeline(const std::string &name,
    const bool allowFallback) {
    if (const auto it = pipelineCache_.find(name); it != pipelineCache_.end()) {
        return it->second;
    }
    //Try to load the pipeline configuration from the core resource pack first.
    //优先从 core 资源包加载管线配置。
    if (resourceLocator_ != nullptr) {
        ResourceRef resourceRef;
        resourceRef.SetPackageId(RESOURCE_REF_CORE);
        resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
        resourceRef.SetResourceType(RESOURCE_PIPELINE);
        resourceRef.SetResourceKey(name);
        if (auto pipelineResult = resourceLocator_->FindPipeline(&resourceRef)) {
            if (const GPUPipelineResource *config = pipelineResult->GetResource()) {
                if (SDL_GPUGraphicsPipeline *pipeline = CreatePipelineFromConfig(*config)) {
                    LogCat::i("Created graphics pipeline from resource: ", name);
                    pipelineCache_[name] = pipeline;
                    return pipeline;
                }
            }
        }
    }
    if (!allowFallback) {
        LogCat::w(std::source_location::current(), "Pipeline resource not found and fallback disabled: ", name);
        return nullptr;
    }
    if (SDL_GPUGraphicsPipeline *pipeline = CreateFallbackPipeline()) {
        LogCat::w(std::source_location::current(), "Created fallback pipeline for: ", name);
        pipelineCache_[name] = pipeline;
        return pipeline;
    }
    return nullptr;
}

SDL_GPUGraphicsPipeline *glimmer::GpuShaderPipelineManager::CreateSpritePipeline(SDL_GPUShader *vertexShader,
    SDL_GPUShader *fragmentShader, SpriteBlendMode blendMode) const {
    SDL_GPUVertexBufferDescription vertexBufferDescription = {};
    vertexBufferDescription.slot = 0;
    vertexBufferDescription.pitch = sizeof(SpriteVertex);
    vertexBufferDescription.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertexBufferDescription.instance_step_rate = 0;

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
    colorTarget.format = colorFormat_;
    switch (blendMode) {
        case SpriteBlendMode::Alpha:
            colorTarget.blend_state.enable_blend = true;
            colorTarget.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
            colorTarget.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
            colorTarget.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
            colorTarget.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
            colorTarget.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
            colorTarget.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
            break;
        case SpriteBlendMode::Multiply:
            colorTarget.blend_state.enable_blend = true;
            colorTarget.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
            colorTarget.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
            colorTarget.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_DST_COLOR;
            colorTarget.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
            colorTarget.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
            colorTarget.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
            break;
        case SpriteBlendMode::None:
            colorTarget.blend_state.enable_blend = false;
            break;
    }

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = &vertexBufferDescription;
    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_attributes = attributes;
    pipelineInfo.vertex_input_state.num_vertex_attributes = 3;
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipelineInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    pipelineInfo.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
    pipelineInfo.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    pipelineInfo.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
    pipelineInfo.target_info.color_target_descriptions = &colorTarget;
    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.has_depth_stencil_target = false;
    pipelineInfo.props = 0;
    SDL_GPUGraphicsPipeline *pipeline = SDL_CreateGPUGraphicsPipeline(device_, &pipelineInfo);
    if (pipeline == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_CreateGPUGraphicsPipeline failed: ", SDL_GetError());
    }
    return pipeline;
}

SDL_GPUGraphicsPipeline *glimmer::GpuShaderPipelineManager::CreatePipelineFromConfig(
    const GPUPipelineResource &config) {
    const std::string vertexKey = config.vertexShader.GetResourceKey();
    const std::string vertexName = vertexKey.substr(0, vertexKey.find_last_of('.'));
    SDL_GPUShader *vertexShader = GetOrCreateShader(vertexName, SHADER_EXTENSION_VERT,
                                                    SDL_GPU_SHADERSTAGE_VERTEX, 0, 1, DEFAULT_SPRITE_VERT);
    const std::string fragmentKey = config.fragmentShader.GetResourceKey();
    const std::string fragmentName = fragmentKey.substr(0, fragmentKey.find_last_of('.'));
    SDL_GPUShader *fragmentShader = GetOrCreateShader(fragmentName, SHADER_EXTENSION_FRAG,
                                                      SDL_GPU_SHADERSTAGE_FRAGMENT,
                                                      config.fragmentNumSamplers, config.fragmentNumUniformBuffers,
                                                      DEFAULT_PASSTHROUGH_FRAG);
    if (vertexShader == nullptr || fragmentShader == nullptr) {
        LogCat::w(std::source_location::current(), "Failed to get shaders for pipeline config");
        return nullptr;
    }
    return CreateSpritePipeline(vertexShader, fragmentShader, static_cast<SpriteBlendMode>(config.blendMode));
}

SDL_GPUGraphicsPipeline *glimmer::GpuShaderPipelineManager::CreateFallbackPipeline() {
    SDL_GPUShader *vertexShader = GetOrCreateShader(SHADER_NAME_SPRITE_VERT, SHADER_EXTENSION_VERT,
                                                    SDL_GPU_SHADERSTAGE_VERTEX, 0, 1, DEFAULT_SPRITE_VERT);
    SDL_GPUShader *fragmentShader = GetOrCreateShader(SHADER_NAME_SPRITE_VERT, SHADER_EXTENSION_FRAG,
                                                      SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 0, DEFAULT_PASSTHROUGH_FRAG);
    if (vertexShader == nullptr || fragmentShader == nullptr) {
        LogCat::w(std::source_location::current(), "Failed to get fallback shaders");
        return nullptr;
    }
    return CreateSpritePipeline(vertexShader, fragmentShader, SpriteBlendMode::Alpha);
}

glimmer::GpuShaderPipelineManager::GpuShaderPipelineManager() = default;

glimmer::GpuShaderPipelineManager::~GpuShaderPipelineManager() = default;

bool glimmer::GpuShaderPipelineManager::Init(SDL_GPUDevice *device, ResourceLocator *locator, VirtualFileSystem *vfs,
                                             const std::filesystem::path &cachePath, SDL_GPUTextureFormat colorFormat) {
    if (device == nullptr) {
        return false;
    }
    device_ = device;
    resourceLocator_ = locator;
    virtualFileSystem_ = vfs;
    colorFormat_ = colorFormat;
    gpuShaderCache_ = std::make_unique<GpuShaderCache>(vfs, cachePath);
    return true;
}

void glimmer::GpuShaderPipelineManager::Shutdown() {
    for (const auto &[name, pipeline]: pipelineCache_) {
        if (pipeline != nullptr) {
            SDL_ReleaseGPUGraphicsPipeline(device_, pipeline);
        }
    }
    pipelineCache_.clear();
    for (const auto &[key, shader]: shaderCache_) {
        if (shader != nullptr) {
            SDL_ReleaseGPUShader(device_, shader);
        }
    }
    shaderCache_.clear();
    gpuShaderCache_.reset();
    device_ = nullptr;
    resourceLocator_ = nullptr;
    virtualFileSystem_ = nullptr;
}

SDL_GPUGraphicsPipeline *glimmer::GpuShaderPipelineManager::GetSpritePipeline() {
    return GetOrCreatePipeline(PIPELINE_NAME_SPRITE, true);
}

SDL_GPUGraphicsPipeline *glimmer::GpuShaderPipelineManager::GetGamePipeline() {
    return GetOrCreatePipeline(PIPELINE_NAME_GAME, false);
}

SDL_GPUGraphicsPipeline *glimmer::GpuShaderPipelineManager::GetUiPipeline() {
    return GetOrCreatePipeline(PIPELINE_NAME_UI, false);
}

SDL_GPUGraphicsPipeline *glimmer::GpuShaderPipelineManager::GetGlobalPipeline() {
    return GetOrCreatePipeline(PIPELINE_NAME_GLOBAL, false);
}

SDL_GPUGraphicsPipeline *glimmer::GpuShaderPipelineManager::GetLightingPipeline() {
    return GetOrCreatePipeline(PIPELINE_NAME_LIGHTING, false);
}
