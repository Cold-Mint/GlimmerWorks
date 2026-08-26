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
#include "core/mod/resourcePack/ShaderResourceResult.h"

namespace {
    constexpr const char *SHADER_NAME_SPRITE_VERT = "sprite";
    constexpr const char *PIPELINE_NAME_SPRITE = "sprite";
    constexpr const char *PIPELINE_NAME_GAME = "game";
    constexpr const char *PIPELINE_NAME_UI = "ui";
    constexpr const char *PIPELINE_NAME_GLOBAL = "global";
    constexpr const char *PIPELINE_NAME_LIGHTING = "lighting";
    constexpr const char *SHADER_EXTENSION_VERT = "vert";
    constexpr const char *SHADER_EXTENSION_FRAG = "frag";

    constexpr const char *DEFAULT_SPRITE_VERT = R"(#version 450
layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec4 in_color;
layout(location = 0) out vec2 out_uv;
layout(location = 1) out vec4 out_color;
layout(set = 1, binding = 0) uniform ProjectionUniform {
    vec2 u_viewSize;
};
void main() {
    vec2 ndc;
    ndc.x = (in_position.x / u_viewSize.x) * 2.0 - 1.0;
    ndc.y = 1.0 - (in_position.y / u_viewSize.y) * 2.0;
    gl_Position = vec4(ndc, 0.0, 1.0);
    out_uv = in_uv;
    out_color = in_color;
}
)";

    constexpr const char *DEFAULT_PASSTHROUGH_FRAG = R"(#version 450
layout(location = 0) in vec2 in_uv;
layout(location = 1) in vec4 in_color;
layout(location = 0) out vec4 out_color;
layout(set = 2, binding = 0) uniform sampler2D inputTexture;
void main() {
    out_color = texture(inputTexture, in_uv) * in_color;
}
)";

    constexpr const char *DEFAULT_LIGHTING_FRAG = R"(#version 450
layout(location = 0) in vec2 in_uv;
layout(location = 1) in vec4 in_color;
layout(location = 0) out vec4 out_color;

layout(set = 2, binding = 0) uniform sampler2D lightMap;

layout(set = 3, binding = 0) uniform LightingUniform {
    vec2 u_lightMapOrigin;
    vec2 u_lightMapSize;
    vec2 u_cameraTopLeftTile;
    vec2 u_viewportTiles;
    float u_fullBright;
    float u_minVisibility;
    float u_tintStrength;
    float u_padding;
};

void main() {
    vec2 tilePos = u_cameraTopLeftTile + vec2(in_uv.x, -in_uv.y) * u_viewportTiles;
    vec2 lightUv = (tilePos - u_lightMapOrigin + 0.5) / u_lightMapSize;
    vec4 light = texture(lightMap, lightUv);

    float visibility = clamp(light.a / max(u_fullBright, 1e-4), 0.0, 1.0);
    float luminance = mix(u_minVisibility, 1.0, visibility);

    float hueMax = max(max(light.r, light.g), light.b);
    vec3 hue = hueMax > 1e-4 ? light.rgb / hueMax : vec3(1.0);

    vec3 multiplier = luminance * mix(vec3(1.0), hue, u_tintStrength);
    out_color = vec4(multiplier, 1.0);
}
)";
}

namespace glimmer {
    GpuShaderPipelineManager::GpuShaderPipelineManager() = default;

    GpuShaderPipelineManager::~GpuShaderPipelineManager() = default;

    bool GpuShaderPipelineManager::Init(SDL_GPUDevice *device, ResourceLocator *locator, VirtualFileSystem *vfs,
                                        const std::filesystem::path &cachePath,
                                        const SDL_GPUTextureFormat colorFormat) {
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

    void GpuShaderPipelineManager::Shutdown() {
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

    SDL_GPUGraphicsPipeline *GpuShaderPipelineManager::CreateSpritePipeline(SDL_GPUShader *vertexShader,
                                                                            SDL_GPUShader *fragmentShader,
                                                                            const SpriteBlendMode blendMode) const {
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

    SDL_GPUShader *GpuShaderPipelineManager::GetOrCreateShader(const std::string &key, const std::string &extension,
                                                               const SDL_GPUShaderStage stage,
                                                               const Uint32 numSamplers,
                                                               const Uint32 numUniformBuffers,
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

    SDL_GPUGraphicsPipeline *GpuShaderPipelineManager::GetOrCreatePipeline(const std::string &name,
                                                                           const SpriteBlendMode blendMode,
                                                                           const Uint32 numSamplers,
                                                                           const Uint32 numUniformBuffers,
                                                                           const char *fallbackFragSource) {
        if (const auto it = pipelineCache_.find(name); it != pipelineCache_.end()) {
            return it->second;
        }
        SDL_GPUShader *vertexShader = GetOrCreateShader(SHADER_NAME_SPRITE_VERT, SHADER_EXTENSION_VERT,
                                                        SDL_GPU_SHADERSTAGE_VERTEX, 0, 1, DEFAULT_SPRITE_VERT);
        SDL_GPUShader *fragmentShader = GetOrCreateShader(name, SHADER_EXTENSION_FRAG, SDL_GPU_SHADERSTAGE_FRAGMENT,
                                                          numSamplers, numUniformBuffers, fallbackFragSource);
        if (vertexShader == nullptr || fragmentShader == nullptr) {
            LogCat::w(std::source_location::current(), "Failed to get shaders for pipeline: ", name);
            return nullptr;
        }
        SDL_GPUGraphicsPipeline *pipeline = CreateSpritePipeline(vertexShader, fragmentShader, blendMode);
        if (pipeline == nullptr) {
            return nullptr;
        }
        LogCat::i("Created graphics pipeline lazily: ", name);
        pipelineCache_[name] = pipeline;
        return pipeline;
    }

    SDL_GPUGraphicsPipeline *GpuShaderPipelineManager::GetSpritePipeline() {
        return GetOrCreatePipeline(PIPELINE_NAME_SPRITE, SpriteBlendMode::Alpha, 1, 0, DEFAULT_PASSTHROUGH_FRAG);
    }

    SDL_GPUGraphicsPipeline *GpuShaderPipelineManager::GetGamePipeline() {
        return GetOrCreatePipeline(PIPELINE_NAME_GAME, SpriteBlendMode::Alpha, 1, 0, DEFAULT_PASSTHROUGH_FRAG);
    }

    SDL_GPUGraphicsPipeline *GpuShaderPipelineManager::GetUiPipeline() {
        return GetOrCreatePipeline(PIPELINE_NAME_UI, SpriteBlendMode::Alpha, 1, 0, DEFAULT_PASSTHROUGH_FRAG);
    }

    SDL_GPUGraphicsPipeline *GpuShaderPipelineManager::GetGlobalPipeline() {
        return GetOrCreatePipeline(PIPELINE_NAME_GLOBAL, SpriteBlendMode::None, 1, 0, DEFAULT_PASSTHROUGH_FRAG);
    }

    SDL_GPUGraphicsPipeline *GpuShaderPipelineManager::GetLightingPipeline() {
        return GetOrCreatePipeline(PIPELINE_NAME_LIGHTING, SpriteBlendMode::Multiply, 1, 1, DEFAULT_LIGHTING_FRAG);
    }
}
