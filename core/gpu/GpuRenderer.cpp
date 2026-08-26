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
#include "GpuRenderer.h"

#include <algorithm>

#include "GpuShaderCache.h"
#include "GpuShaderCompiler.h"
#include "RenderQueue.h"
#include "core/config/Config.h"
#include "core/config/Constants.h"
#include "core/context/AppContext.h"
#include "core/log/LogCat.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/ResourceRef.h"
#include "core/mod/resourcePack/ShaderResourceResult.h"

namespace {
    //Shader names inside the resource packs (directory shaders/@core/). Each
    //pipeline name doubles as its fragment shader name (e.g. pipeline "game"
    //uses shaders/@core/game.frag); all pipelines share the "sprite" vertex shader.
    //材质包内的着色器名（目录 shaders/@core/）。管线名同时作为其片元着色器名
    //（如管线 "game" 使用 shaders/@core/game.frag）；所有管线共享 "sprite" 顶点着色器。
    constexpr const char *SHADER_NAME_SPRITE_VERT = "sprite";
    constexpr const char *PIPELINE_NAME_SPRITE = "sprite";
    constexpr const char *PIPELINE_NAME_GAME = "game";
    constexpr const char *PIPELINE_NAME_UI = "ui";
    constexpr const char *PIPELINE_NAME_GLOBAL = "global";
    constexpr const char *PIPELINE_NAME_LIGHTING = "lighting";
    constexpr const char *SHADER_EXTENSION_VERT = "vert";
    constexpr const char *SHADER_EXTENSION_FRAG = "frag";

    /**
     * Embedded fallback vertex shader (used only when the resource packs do
     * not provide one). Converts pixel coordinates (top-left origin) to NDC.
     * 内嵌兜底顶点着色器（仅当材质包未提供时使用）。将像素坐标（左上角原点）转换为 NDC。
     */
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

    /**
     * Embedded pass-through fragment shader (default for all four pipelines
     * when the resource packs do not override them). Keeps the original
     * visual output: texture multiplied by the vertex color.
     * 内嵌 pass-through 片元着色器（材质包未覆盖时四条管线的默认实现）。
     * 保持原始视觉输出：纹理乘以顶点颜色。
     */
    constexpr const char *DEFAULT_PASSTHROUGH_FRAG = R"(#version 450
layout(location = 0) in vec2 in_uv;
layout(location = 1) in vec4 in_color;
layout(location = 0) out vec4 out_color;
layout(set = 2, binding = 0) uniform sampler2D inputTexture;
void main() {
    out_color = texture(inputTexture, in_uv) * in_color;
}
)";

    /**
     * Embedded fallback lighting fragment shader (used only when the resource
     * packs do not provide shaders/@core/lighting.frag).
     * 内嵌兜底光照片元着色器（仅当材质包未提供 shaders/@core/lighting.frag 时使用）。
     *
     * 2D physically based lighting (Lambertian diffuse): the output of this
     * pass is blended onto the game layer with multiplicative blending
     * (DST_COLOR), so every scene pixel becomes albedo * irradiance, which
     * conserves energy (the multiplier never exceeds 1) and tints surfaces
     * by the light hue like a physical color filter.
     * 2D 基于物理的光照（朗伯漫反射）：本通道的输出通过乘法混合（DST_COLOR）
     * 作用到 game 层，因此每个场景像素变为 反照率 × 辐照度，既能量守恒
     * （系数永不超过 1），又像物理滤色片一样用光色对表面染色。
     */
    constexpr const char *DEFAULT_LIGHTING_FRAG = R"(#version 450
layout(location = 0) in vec2 in_uv;
layout(location = 1) in vec4 in_color;
layout(location = 0) out vec4 out_color;

// Per-tile light map written by the lighting system: rgb = light hue, a = intensity.
// 光照系统写入的逐瓦片光照贴图：rgb = 光色，a = 强度。
layout(set = 2, binding = 0) uniform sampler2D lightMap;

// Must match glimmer::LightMapParams (std140 layout).
// 必须与 glimmer::LightMapParams 一致（std140 布局）。
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
    // Screen UV -> continuous tile coordinate. World +Y points up while the
    // screen +Y points down, hence the negated y component.
    // 屏幕 UV -> 连续瓦片坐标。世界 Y 轴向上而屏幕 Y 轴向下，因此 y 分量取负。
    vec2 tilePos = u_cameraTopLeftTile + vec2(in_uv.x, -in_uv.y) * u_viewportTiles;
    // +0.5 aligns the sample with tile centers so the linear sampler
    // interpolates smoothly across tile borders (no more blocky light spots).
    // +0.5 将采样点对齐到瓦片中心，使线性采样器在瓦片边界平滑插值
    // （消除方块状光斑）。
    vec2 lightUv = (tilePos - u_lightMapOrigin + 0.5) / u_lightMapSize;
    vec4 light = texture(lightMap, lightUv);

    // Visibility term: u_fullBright intensity counts as fully lit, anything
    // below fades smoothly towards the ambient floor.
    // 可见度项：u_fullBright 强度视为全亮，低于它的区域向环境光下限平滑过渡。
    float visibility = clamp(light.a / max(u_fullBright, 1e-4), 0.0, 1.0);
    float luminance = mix(u_minVisibility, 1.0, visibility);

    // Normalized light hue (dominant channel = 1) so tinting never darkens.
    // 归一化光色（主通道 = 1），保证染色本身不会变暗。
    float hueMax = max(max(light.r, light.g), light.b);
    vec3 hue = hueMax > 1e-4 ? light.rgb / hueMax : vec3(1.0);

    // Lambertian irradiance multiplier: outgoing = albedo * E.
    // 朗伯辐照系数：出射光 = 反照率 × 辐照度。
    vec3 multiplier = luminance * mix(vec3(1.0), hue, u_tintStrength);
    out_color = vec4(multiplier, 1.0);
}
)";
}

glimmer::GpuRenderer::~GpuRenderer() {
    Shutdown();
}

SDL_GPUGraphicsPipeline *glimmer::GpuRenderer::CreateSpritePipeline(SDL_GPUShader *vertexShader,
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
    colorTarget.format = gpuContext_->GetSwapchainFormat();
    switch (blendMode) {
        case SpriteBlendMode::Alpha:
            //Standard alpha blending, equivalent to SDL_BLENDMODE_BLEND.
            //标准 alpha 混合，等价于 SDL_BLENDMODE_BLEND。
            colorTarget.blend_state.enable_blend = true;
            colorTarget.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
            colorTarget.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
            colorTarget.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
            colorTarget.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
            colorTarget.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
            colorTarget.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
            break;
        case SpriteBlendMode::Multiply:
            //Multiplicative blending: result.rgb = dst.rgb * src.rgb, the
            //destination alpha is preserved. Used by the lighting pass.
            //乘法混合：result.rgb = dst.rgb * src.rgb，保留目标 alpha。光照通道使用。
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

SDL_GPUShader *glimmer::GpuRenderer::GetOrCreateShader(const std::string &key, const std::string &extension,
                                                       const SDL_GPUShaderStage stage, const Uint32 numSamplers,
                                                       const Uint32 numUniformBuffers, const char *fallbackSource) {
    const std::string shaderKey = key + "." + extension;
    if (const auto it = shaderCache_.find(shaderKey); it != shaderCache_.end()) {
        //Already loaded: return the cached pointer without any disk IO.
        //已加载过：直接返回缓存指针，不再进行磁盘 IO。
        return it->second;
    }
    if (resourceLocator_ != nullptr) {
        //Locate the shader source through the ResourceLocator (RESOURCE_SHADER).
        //通过资源定位器（RESOURCE_SHADER）查找着色器源码。
        ResourceRef resourceRef;
        resourceRef.SetPackageId(RESOURCE_REF_CORE);
        resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
        resourceRef.SetResourceType(RESOURCE_SHADER);
        resourceRef.SetResourceKey(shaderKey);
        if (auto shaderResult = resourceLocator_->FindShader(&resourceRef); shaderResult != nullptr) {
            SDL_GPUShader *shader = nullptr;
            if (gpuShaderCache_ != nullptr) {
                //Fast path: reuse the cached SPIR-V binary when the source
                //file did not change (mtime, confirmed by blake3).
                //快速路径：源文件未变化时复用缓存的 SPIR-V 二进制
                //（修改时间判断，blake3 哈希确认）。
                if (auto spirv = gpuShaderCache_->TryLoad(&resourceRef, shaderResult->GetPath(),
                                                          shaderResult->GetSource());
                    spirv.has_value()) {
                    shader = GpuShaderCompiler::CreateFromSpirv(device_, spirv->data(), spirv->size(),
                                                                shaderKey.c_str(), stage, numSamplers,
                                                                numUniformBuffers);
                    //If the driver rejects the cached binary, fall through and recompile.
                    //若驱动拒绝缓存的二进制，继续向下重新编译。
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
                                                                spirv.size() * sizeof(unsigned int), shaderKey.c_str(),
                                                                stage, numSamplers, numUniformBuffers);
                }
            }
            if (shader != nullptr) {
                shaderCache_[shaderKey] = shader;
                return shader;
            }
            LogCat::w(std::source_location::current(), "Pack shader failed to compile, using fallback: ", shaderKey);
        }
    }
    SDL_GPUShader *fallbackShader = GpuShaderCompiler::CompileFromSource(
        device_, fallbackSource, (shaderKey + "(fallback)").c_str(), stage, numSamplers, numUniformBuffers);
    if (fallbackShader != nullptr) {
        shaderCache_[shaderKey] = fallbackShader;
    }
    return fallbackShader;
}

SDL_GPUGraphicsPipeline *glimmer::GpuRenderer::GetOrCreatePipeline(const std::string &name,
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

SDL_GPUGraphicsPipeline *glimmer::GpuRenderer::GetSpritePipeline() {
    return GetOrCreatePipeline(PIPELINE_NAME_SPRITE, SpriteBlendMode::Alpha, 1, 0, DEFAULT_PASSTHROUGH_FRAG);
}

SDL_GPUGraphicsPipeline *glimmer::GpuRenderer::GetGamePipeline() {
    return GetOrCreatePipeline(PIPELINE_NAME_GAME, SpriteBlendMode::Alpha, 1, 0, DEFAULT_PASSTHROUGH_FRAG);
}

SDL_GPUGraphicsPipeline *glimmer::GpuRenderer::GetUiPipeline() {
    return GetOrCreatePipeline(PIPELINE_NAME_UI, SpriteBlendMode::Alpha, 1, 0, DEFAULT_PASSTHROUGH_FRAG);
}

SDL_GPUGraphicsPipeline *glimmer::GpuRenderer::GetGlobalPipeline() {
    return GetOrCreatePipeline(PIPELINE_NAME_GLOBAL, SpriteBlendMode::None, 1, 0, DEFAULT_PASSTHROUGH_FRAG);
}

SDL_GPUGraphicsPipeline *glimmer::GpuRenderer::GetLightingPipeline() {
    //The lighting shader samples the light map (1 sampler) and receives the
    //LightMapParams uniform block (1 fragment uniform buffer).
    //光照着色器采样光照贴图（1 个采样器）并接收 LightMapParams
    //uniform 块（1 个片元 uniform 缓冲）。
    return GetOrCreatePipeline(PIPELINE_NAME_LIGHTING, SpriteBlendMode::Multiply, 1, 1, DEFAULT_LIGHTING_FRAG);
}

bool glimmer::GpuRenderer::Init(GpuContext *gpuContext, AppContext *appContext) {
    if (gpuContext == nullptr || gpuContext->GetDevice() == nullptr) {
        LogCat::w(std::source_location::current(), "gpuContext is nullptr or not initialized");
        return false;
    }
    if (appContext == nullptr) {
        LogCat::w(std::source_location::current(), "appContext is nullptr");
        return false;
    }
    gpuContext_ = gpuContext;
    device_ = gpuContext->GetDevice();
    resourceLocator_ = appContext->GetResourceLocator();
    virtualFileSystem_ = appContext->GetVirtualFileSystem();
    //Prepare the shader disk cache. Shaders and pipelines are NOT compiled
    //here; they are created lazily on first use (see GetOrCreatePipeline).
    //准备好着色器磁盘缓存。此处不编译着色器和管线；它们在首次使用时惰性创建
    //（见 GetOrCreatePipeline）。
    const Config *config = appContext->GetConfig();
    gpuShaderCache_ = std::make_unique<GpuShaderCache>(virtualFileSystem_,
                                                       config != nullptr ? std::filesystem::path(config->cachePath)
                                                                         : std::filesystem::path(".cache"));

    SDL_GPUBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    bufferCreateInfo.size = INITIAL_VERTEX_CAPACITY * sizeof(SpriteVertex);
    bufferCreateInfo.props = 0;
    vertexBuffer_ = SDL_CreateGPUBuffer(device_, &bufferCreateInfo);
    if (vertexBuffer_ == nullptr) {
        LogCat::e(std::source_location::current(), "SDL_CreateGPUBuffer failed: ", SDL_GetError());
        Shutdown();
        return false;
    }
    vertexBufferCapacity_ = INITIAL_VERTEX_CAPACITY;

    //Static full-screen unit quad used by the layer composite passes. Positions
    //double as UVs; the projection uniform is (1,1) so it covers the target.
    //分层合成通道使用的静态全屏单位四边形。位置即 UV；投影 uniform 为 (1,1)，覆盖整个目标。
    const SpriteVertex unitQuadVertices[6] = {
        {0.0F, 0.0F, 0.0F, 0.0F, 255, 255, 255, 255},
        {1.0F, 0.0F, 1.0F, 0.0F, 255, 255, 255, 255},
        {0.0F, 1.0F, 0.0F, 1.0F, 255, 255, 255, 255},
        {1.0F, 0.0F, 1.0F, 0.0F, 255, 255, 255, 255},
        {1.0F, 1.0F, 1.0F, 1.0F, 255, 255, 255, 255},
        {0.0F, 1.0F, 0.0F, 1.0F, 255, 255, 255, 255}
    };
    SDL_GPUBufferCreateInfo unitQuadBufferCreateInfo = {};
    unitQuadBufferCreateInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    unitQuadBufferCreateInfo.size = sizeof(unitQuadVertices);
    unitQuadBufferCreateInfo.props = 0;
    unitQuadBuffer_ = SDL_CreateGPUBuffer(device_, &unitQuadBufferCreateInfo);
    if (unitQuadBuffer_ == nullptr) {
        LogCat::e(std::source_location::current(), "SDL_CreateGPUBuffer(unit quad) failed: ", SDL_GetError());
        Shutdown();
        return false;
    }
    {
        SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {};
        transferBufferCreateInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transferBufferCreateInfo.size = sizeof(unitQuadVertices);
        transferBufferCreateInfo.props = 0;
        SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(device_, &transferBufferCreateInfo);
        if (transferBuffer == nullptr) {
            LogCat::e(std::source_location::current(), "SDL_CreateGPUTransferBuffer failed: ", SDL_GetError());
            Shutdown();
            return false;
        }
        void *mapped = SDL_MapGPUTransferBuffer(device_, transferBuffer, false);
        if (mapped == nullptr) {
            SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
            LogCat::e(std::source_location::current(), "SDL_MapGPUTransferBuffer failed: ", SDL_GetError());
            Shutdown();
            return false;
        }
        SDL_memcpy(mapped, unitQuadVertices, sizeof(unitQuadVertices));
        SDL_UnmapGPUTransferBuffer(device_, transferBuffer);
        SDL_GPUCommandBuffer *uploadCommandBuffer = SDL_AcquireGPUCommandBuffer(device_);
        SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(uploadCommandBuffer);
        SDL_GPUTransferBufferLocation location = {};
        location.transfer_buffer = transferBuffer;
        location.offset = 0;
        SDL_GPUBufferRegion region = {};
        region.buffer = unitQuadBuffer_;
        region.offset = 0;
        region.size = sizeof(unitQuadVertices);
        SDL_UploadToGPUBuffer(copyPass, &location, &region, false);
        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(uploadCommandBuffer);
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
    }

    //1x1 white texture used to draw plain colored geometry (rects/lines/points).
    //1x1 白色纹理，用于绘制纯色几何图形（矩形/线/点）。
    SDL_Surface *whiteSurface = SDL_CreateSurface(1, 1, SDL_PIXELFORMAT_ABGR8888);
    if (whiteSurface == nullptr) {
        LogCat::e(std::source_location::current(), "SDL_CreateSurface failed: ", SDL_GetError());
        Shutdown();
        return false;
    }
    SDL_FillSurfaceRect(whiteSurface, nullptr, SDL_MapSurfaceRGBA(whiteSurface, 255, 255, 255, 255));
    whiteTexture_.reset(gpuContext_->CreateTextureFromSurface(whiteSurface));
    SDL_DestroySurface(whiteSurface);
    if (whiteTexture_ == nullptr) {
        LogCat::e(std::source_location::current(), "Failed to create white texture");
        Shutdown();
        return false;
    }
    LogCat::i("GpuRenderer initialized");
    return true;
}

void glimmer::GpuRenderer::Shutdown() {
    if (device_ != nullptr) {
        whiteTexture_.reset();
        compositeLayerTexture_.reset();
        uiLayerTexture_.reset();
        gameLayerTexture_.reset();
        lightMapTexture_.reset();
        lightMapPixels_.clear();
        lightMapPixels_.shrink_to_fit();
        lightMapWidth_ = 0;
        lightMapHeight_ = 0;
        lightMapPending_ = false;
        lightMapDirty_ = false;
        if (unitQuadBuffer_ != nullptr) {
            SDL_ReleaseGPUBuffer(device_, unitQuadBuffer_);
            unitQuadBuffer_ = nullptr;
        }
        if (vertexBuffer_ != nullptr) {
            SDL_ReleaseGPUBuffer(device_, vertexBuffer_);
            vertexBuffer_ = nullptr;
        }
        vertexBufferCapacity_ = 0;
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
    }
    device_ = nullptr;
    gpuContext_ = nullptr;
    resourceLocator_ = nullptr;
    virtualFileSystem_ = nullptr;
}

bool glimmer::GpuRenderer::EnsureLayerTextures() {
    if (gameLayerTexture_ != nullptr && uiLayerTexture_ != nullptr && compositeLayerTexture_ != nullptr &&
        gameLayerTexture_->w == static_cast<int>(swapchainWidth_) &&
        gameLayerTexture_->h == static_cast<int>(swapchainHeight_)) {
        return true;
    }
    //The old textures are released here; SDL defers the actual destruction
    //until in-flight frames no longer reference them.
    //此处释放旧纹理；SDL 会将实际销毁延迟到在途帧不再引用它们之后。
    gameLayerTexture_.reset();
    uiLayerTexture_.reset();
    compositeLayerTexture_.reset();
    const SDL_GPUTextureFormat format = gpuContext_->GetSwapchainFormat();
    gameLayerTexture_.reset(gpuContext_->CreateTargetTexture(swapchainWidth_, swapchainHeight_, format));
    uiLayerTexture_.reset(gpuContext_->CreateTargetTexture(swapchainWidth_, swapchainHeight_, format));
    compositeLayerTexture_.reset(gpuContext_->CreateTargetTexture(swapchainWidth_, swapchainHeight_, format));
    if (gameLayerTexture_ == nullptr || uiLayerTexture_ == nullptr || compositeLayerTexture_ == nullptr) {
        LogCat::w(std::source_location::current(), "Failed to create layer textures");
        return false;
    }
    LogCat::i("Layer textures created: ", static_cast<int>(swapchainWidth_), "x",
              static_cast<int>(swapchainHeight_));
    return true;
}

bool glimmer::GpuRenderer::BeginFrame(SDL_Window *window) {
    if (device_ == nullptr) {
        return false;
    }
    commandBuffer_ = SDL_AcquireGPUCommandBuffer(device_);
    if (commandBuffer_ == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_AcquireGPUCommandBuffer failed: ", SDL_GetError());
        return false;
    }
    swapchainTexture_ = nullptr;
    if (!SDL_AcquireGPUSwapchainTexture(commandBuffer_, window, &swapchainTexture_, &swapchainWidth_,
                                        &swapchainHeight_)) {
        LogCat::w(std::source_location::current(), "SDL_AcquireGPUSwapchainTexture failed: ", SDL_GetError());
        SDL_CancelGPUCommandBuffer(commandBuffer_);
        commandBuffer_ = nullptr;
        return false;
    }
    if (swapchainTexture_ == nullptr) {
        //Too many frames in flight or window minimized: skip drawing this frame
        //but still submit the command buffer in SubmitFrame().
        //帧在飞行中过多或窗口最小化：跳过本帧绘制，但仍在 SubmitFrame() 中提交命令缓冲。
        frameActive_ = false;
        return false;
    }
    frameActive_ = true;
    if (!EnsureLayerTextures()) {
        frameActive_ = false;
        return false;
    }
    //Clear the ui layer to fully transparent; RmlUi later renders into it with
    //LOADOP_LOAD.
    //将 ui 层清除为全透明；RmlUi 随后以 LOADOP_LOAD 渲染进去。
    {
        SDL_GPUColorTargetInfo uiClearTarget = {};
        uiClearTarget.texture = uiLayerTexture_->GetGpuTexture();
        uiClearTarget.mip_level = 0;
        uiClearTarget.layer_or_depth_plane = 0;
        uiClearTarget.load_op = SDL_GPU_LOADOP_CLEAR;
        uiClearTarget.clear_color = {0.0F, 0.0F, 0.0F, 0.0F};
        uiClearTarget.store_op = SDL_GPU_STOREOP_STORE;
        uiClearTarget.cycle = false;
        SDL_GPURenderPass *uiClearPass = SDL_BeginGPURenderPass(commandBuffer_, &uiClearTarget, 1, nullptr);
        if (uiClearPass != nullptr) {
            SDL_EndGPURenderPass(uiClearPass);
        }
    }
    currentTarget_ = gameLayerTexture_->GetGpuTexture();
    EnsureRenderPass(true);
    return true;
}

void glimmer::GpuRenderer::EnsureRenderPass(const bool clear) {
    if (renderPass_ != nullptr || !frameActive_ || currentTarget_ == nullptr) {
        return;
    }
    SDL_GPUColorTargetInfo colorTargetInfo = {};
    colorTargetInfo.texture = currentTarget_;
    colorTargetInfo.mip_level = 0;
    colorTargetInfo.layer_or_depth_plane = 0;
    colorTargetInfo.load_op = clear ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
    colorTargetInfo.clear_color = {
        static_cast<float>(clearColor_.r) / 255.0F,
        static_cast<float>(clearColor_.g) / 255.0F,
        static_cast<float>(clearColor_.b) / 255.0F,
        static_cast<float>(clearColor_.a) / 255.0F
    };
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.cycle = false;
    renderPass_ = SDL_BeginGPURenderPass(commandBuffer_, &colorTargetInfo, 1, nullptr);
    if (renderPass_ == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_BeginGPURenderPass failed: ", SDL_GetError());
    }
}

void glimmer::GpuRenderer::EndActivePass() {
    if (renderPass_ != nullptr) {
        SDL_EndGPURenderPass(renderPass_);
        renderPass_ = nullptr;
    }
}

void glimmer::GpuRenderer::DrawLayerQuad(SDL_GPUGraphicsPipeline *pipeline, const GpuTexture *source) {
    if (renderPass_ == nullptr || pipeline == nullptr || source == nullptr || !source->IsValid()) {
        return;
    }
    SDL_BindGPUGraphicsPipeline(renderPass_, pipeline);
    SDL_GPUViewport viewport = {};
    viewport.x = 0.0F;
    viewport.y = 0.0F;
    viewport.w = static_cast<float>(swapchainWidth_);
    viewport.h = static_cast<float>(swapchainHeight_);
    viewport.min_depth = 0.0F;
    viewport.max_depth = 1.0F;
    SDL_SetGPUViewport(renderPass_, &viewport);
    const float viewSize[2] = {1.0F, 1.0F};
    SDL_PushGPUVertexUniformData(commandBuffer_, 0, viewSize, sizeof(viewSize));
    SDL_GPUBufferBinding vertexBufferBinding = {};
    vertexBufferBinding.buffer = unitQuadBuffer_;
    vertexBufferBinding.offset = 0;
    SDL_BindGPUVertexBuffers(renderPass_, 0, &vertexBufferBinding, 1);
    SDL_GPUTextureSamplerBinding textureBinding = {};
    textureBinding.texture = source->GetGpuTexture();
    textureBinding.sampler = gpuContext_->GetNearestSampler();
    SDL_BindGPUFragmentSamplers(renderPass_, 0, &textureBinding, 1);
    SDL_DrawGPUPrimitives(renderPass_, 6, 1, 0, 0);
}

void glimmer::GpuRenderer::ExpandCommands(const RenderCommand *commands, const size_t count,
                                          std::vector<SpriteVertex> &vertices, std::vector<DrawRun> &runs) const {
    //Expand every command into two triangles and group consecutive commands
    //sharing the same texture into draw runs.
    //把每个命令展开为两个三角形，并把共享同一纹理的连续命令归入绘制段。
    SDL_GPUTexture *currentTexture = nullptr;
    for (size_t i = 0; i < count; ++i) {
        const RenderCommand &command = commands[i];
        SDL_GPUTexture *commandTexture = command.texture != nullptr
                                             ? command.texture->GetGpuTexture()
                                             : (whiteTexture_ != nullptr ? whiteTexture_->GetGpuTexture() : nullptr);
        if (commandTexture == nullptr) {
            continue;
        }
        if (currentTexture != commandTexture) {
            currentTexture = commandTexture;
            runs.push_back({commandTexture, static_cast<Uint32>(vertices.size()), 0});
        }
        const SpriteVertex &topLeft = command.corners[0];
        const SpriteVertex &topRight = command.corners[1];
        const SpriteVertex &bottomLeft = command.corners[2];
        const SpriteVertex &bottomRight = command.corners[3];
        vertices.push_back(topLeft);
        vertices.push_back(topRight);
        vertices.push_back(bottomLeft);
        vertices.push_back(topRight);
        vertices.push_back(bottomRight);
        vertices.push_back(bottomLeft);
        runs.back().vertexCount += 6;
    }
}

void glimmer::GpuRenderer::SetLightMap(const int width, const int height, const Uint8 *rgbaPixels,
                                       const LightMapParams &params) {
    if (width <= 0 || height <= 0) {
        return;
    }
    lightMapPending_ = true;
    lightMapParams_ = params;
    if (rgbaPixels == nullptr) {
        //Reuse the GPU texture contents from the previous frame (the camera
        //moved within the same tile area and the light data did not change).
        //复用上一帧的 GPU 纹理内容（相机在同一瓦片区域内移动且光照数据未变化）。
        return;
    }
    const size_t dataSize = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;
    if (lightMapPixels_.size() != dataSize) {
        lightMapPixels_.resize(dataSize);
    }
    SDL_memcpy(lightMapPixels_.data(), rgbaPixels, dataSize);
    lightMapWidth_ = width;
    lightMapHeight_ = height;
    lightMapDirty_ = true;
}

bool glimmer::GpuRenderer::EnsureLightMapUploaded() {
    if (!lightMapDirty_) {
        return lightMapTexture_ != nullptr && lightMapTexture_->IsValid();
    }
    lightMapDirty_ = false;
    if (lightMapTexture_ == nullptr || lightMapTexture_->w != lightMapWidth_ ||
        lightMapTexture_->h != lightMapHeight_) {
        //The old texture is released here; SDL defers the actual destruction
        //until in-flight frames no longer reference it.
        //此处释放旧纹理；SDL 会将实际销毁延迟到在途帧不再引用它之后。
        lightMapTexture_.reset();
        lightMapTexture_.reset(gpuContext_->CreateSampledTexture(static_cast<Uint32>(lightMapWidth_),
                                                                 static_cast<Uint32>(lightMapHeight_),
                                                                 SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM));
        if (lightMapTexture_ == nullptr) {
            LogCat::w(std::source_location::current(), "Failed to create light map texture");
            return false;
        }
    }
    const Uint32 dataSize = static_cast<Uint32>(lightMapPixels_.size());
    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {};
    transferBufferCreateInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferBufferCreateInfo.size = dataSize;
    transferBufferCreateInfo.props = 0;
    SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(device_, &transferBufferCreateInfo);
    if (transferBuffer == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_CreateGPUTransferBuffer(light map) failed: ", SDL_GetError());
        return false;
    }
    void *mapped = SDL_MapGPUTransferBuffer(device_, transferBuffer, false);
    if (mapped == nullptr) {
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
        LogCat::w(std::source_location::current(), "SDL_MapGPUTransferBuffer(light map) failed: ", SDL_GetError());
        return false;
    }
    SDL_memcpy(mapped, lightMapPixels_.data(), dataSize);
    SDL_UnmapGPUTransferBuffer(device_, transferBuffer);
    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer_);
    if (copyPass == nullptr) {
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
        LogCat::w(std::source_location::current(), "SDL_BeginGPUCopyPass(light map) failed: ", SDL_GetError());
        return false;
    }
    SDL_GPUTextureTransferInfo transferInfo = {};
    transferInfo.transfer_buffer = transferBuffer;
    transferInfo.offset = 0;
    transferInfo.pixels_per_row = static_cast<Uint32>(lightMapWidth_);
    transferInfo.rows_per_layer = static_cast<Uint32>(lightMapHeight_);
    SDL_GPUTextureRegion textureRegion = {};
    textureRegion.texture = lightMapTexture_->GetGpuTexture();
    textureRegion.mip_level = 0;
    textureRegion.layer = 0;
    textureRegion.x = 0;
    textureRegion.y = 0;
    textureRegion.w = static_cast<Uint32>(lightMapWidth_);
    textureRegion.h = static_cast<Uint32>(lightMapHeight_);
    textureRegion.d = 1;
    SDL_UploadToGPUTexture(copyPass, &transferInfo, &textureRegion, false);
    SDL_EndGPUCopyPass(copyPass);
    //Safe to release right after the copy pass is enqueued; SDL defers
    //destruction until the command buffer completes.
    //拷贝通道入队后即可安全释放；SDL 会将销毁延迟到命令缓冲完成。
    SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
    return true;
}

void glimmer::GpuRenderer::DrawLightingQuad() {
    //The lighting pipeline is compiled lazily on the first frame that
    //actually needs it.
    //光照管线在首次真正需要它的那一帧惰性编译。
    SDL_GPUGraphicsPipeline *lightingPipeline = GetLightingPipeline();
    if (renderPass_ == nullptr || lightingPipeline == nullptr || lightMapTexture_ == nullptr ||
        !lightMapTexture_->IsValid()) {
        return;
    }
    SDL_BindGPUGraphicsPipeline(renderPass_, lightingPipeline);
    const float viewSize[2] = {1.0F, 1.0F};
    SDL_PushGPUVertexUniformData(commandBuffer_, 0, viewSize, sizeof(viewSize));
    SDL_PushGPUFragmentUniformData(commandBuffer_, 0, &lightMapParams_, sizeof(LightMapParams));
    SDL_GPUBufferBinding vertexBufferBinding = {};
    vertexBufferBinding.buffer = unitQuadBuffer_;
    vertexBufferBinding.offset = 0;
    SDL_BindGPUVertexBuffers(renderPass_, 0, &vertexBufferBinding, 1);
    //The linear sampler turns the per-tile light map into smooth per-pixel
    //gradients (bilinear interpolation across tile borders).
    //线性采样器把逐瓦片光照贴图变成平滑的逐像素渐变（跨瓦片双线性插值）。
    SDL_GPUTextureSamplerBinding textureBinding = {};
    textureBinding.texture = lightMapTexture_->GetGpuTexture();
    textureBinding.sampler = gpuContext_->GetLinearSampler();
    SDL_BindGPUFragmentSamplers(renderPass_, 0, &textureBinding, 1);
    SDL_DrawGPUPrimitives(renderPass_, 6, 1, 0, 0);
}

void glimmer::GpuRenderer::FlushQueue(RenderQueue &queue) {
    //Consume the light map request even when the frame is inactive so a
    //stale request never leaks into the next frame.
    //即使帧未激活也消费光照贴图请求，避免过期请求泄漏到下一帧。
    const bool lightingRequested = lightMapPending_;
    lightMapPending_ = false;
    if (!frameActive_) {
        return;
    }
    //Sort layer by layer (ascending), then by depth inside each layer;
    //commands with equal keys keep their submission order.
    //逐层排序（升序），层内再按 depth 排序；排序键相等的命令保持提交顺序。
    queue.Sort();
    const std::vector<RenderCommand> &commands = queue.GetCommands();
    if (commands.empty()) {
        EndActivePass();
        return;
    }
    //When lighting is active, the queue is split at the RenderLayer::Lighting
    //boundary: world content is drawn first, then the lighting multiply quad,
    //then debug/overlay content (which must stay unshaded).
    //光照激活时，队列在 RenderLayer::Lighting 边界处拆分：先绘制世界内容，
    //然后绘制光照乘法四边形，最后绘制调试/覆盖层内容（它们必须保持无光照）。
    size_t splitIndex = commands.size();
    if (lightingRequested) {
        const auto splitIterator = std::upper_bound(commands.begin(), commands.end(), RenderLayer::Lighting,
                                                    [](const RenderLayer layer, const RenderCommand &command) {
                                                        return layer < command.layer;
                                                    });
        splitIndex = static_cast<size_t>(std::distance(commands.begin(), splitIterator));
    }
    std::vector<SpriteVertex> vertices;
    vertices.reserve(commands.size() * 6);
    std::vector<DrawRun> worldRuns;
    std::vector<DrawRun> overlayRuns;
    ExpandCommands(commands.data(), splitIndex, vertices, worldRuns);
    ExpandCommands(commands.data() + splitIndex, commands.size() - splitIndex, vertices, overlayRuns);
    EndActivePass();
    //Upload the light map while no render pass is active (copy passes cannot
    //be recorded inside a render pass).
    //在没有活动渲染通道时上传光照贴图（拷贝通道无法在渲染通道内记录）。
    const bool lightingReady = lightingRequested && EnsureLightMapUploaded();
    if (vertices.empty()) {
        if (lightingReady) {
            EnsureRenderPass(false);
            DrawLightingQuad();
            EndActivePass();
        }
        return;
    }
    //Grow the GPU vertex buffer once if the batch does not fit.
    //批次放不下时一次性扩充 GPU 顶点缓冲。
    if (vertices.size() > vertexBufferCapacity_) {
        Uint32 newCapacity = vertexBufferCapacity_ == 0 ? INITIAL_VERTEX_CAPACITY : vertexBufferCapacity_;
        while (newCapacity < vertices.size()) {
            newCapacity *= 2;
        }
        SDL_GPUBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
        bufferCreateInfo.size = newCapacity * sizeof(SpriteVertex);
        bufferCreateInfo.props = 0;
        SDL_GPUBuffer *newBuffer = SDL_CreateGPUBuffer(device_, &bufferCreateInfo);
        if (newBuffer == nullptr) {
            LogCat::w(std::source_location::current(), "Failed to grow vertex buffer: ", SDL_GetError());
        } else {
            SDL_ReleaseGPUBuffer(device_, vertexBuffer_);
            vertexBuffer_ = newBuffer;
            vertexBufferCapacity_ = newCapacity;
        }
    }
    if (vertexBuffer_ == nullptr || vertices.size() > vertexBufferCapacity_) {
        //The buffer could not be grown large enough; skip this batch rather
        //than overflowing the GPU buffer.
        //缓冲无法扩充到足够大；跳过本批次以避免溢出 GPU 缓冲。
        return;
    }
    const Uint32 dataSize = static_cast<Uint32>(vertices.size() * sizeof(SpriteVertex));
    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {};
    transferBufferCreateInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferBufferCreateInfo.size = dataSize;
    transferBufferCreateInfo.props = 0;
    SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(device_, &transferBufferCreateInfo);
    if (transferBuffer == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_CreateGPUTransferBuffer failed: ", SDL_GetError());
        return;
    }
    void *mapped = SDL_MapGPUTransferBuffer(device_, transferBuffer, false);
    if (mapped == nullptr) {
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
        LogCat::w(std::source_location::current(), "SDL_MapGPUTransferBuffer failed: ", SDL_GetError());
        return;
    }
    SDL_memcpy(mapped, vertices.data(), dataSize);
    SDL_UnmapGPUTransferBuffer(device_, transferBuffer);
    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer_);
    if (copyPass != nullptr) {
        SDL_GPUTransferBufferLocation location = {};
        location.transfer_buffer = transferBuffer;
        location.offset = 0;
        SDL_GPUBufferRegion region = {};
        region.buffer = vertexBuffer_;
        region.offset = 0;
        region.size = dataSize;
        SDL_UploadToGPUBuffer(copyPass, &location, &region, false);
        SDL_EndGPUCopyPass(copyPass);
        EnsureRenderPass(false);
        //The sprite pipeline is compiled lazily on the first frame that
        //actually draws something.
        //精灵管线在首次真正绘制内容的那一帧惰性编译。
        SDL_GPUGraphicsPipeline *spritePipeline = GetSpritePipeline();
        if (renderPass_ != nullptr && spritePipeline != nullptr) {
            SDL_BindGPUGraphicsPipeline(renderPass_, spritePipeline);
            SDL_GPUViewport viewport = {};
            viewport.x = 0.0F;
            viewport.y = 0.0F;
            viewport.w = static_cast<float>(swapchainWidth_);
            viewport.h = static_cast<float>(swapchainHeight_);
            viewport.min_depth = 0.0F;
            viewport.max_depth = 1.0F;
            SDL_SetGPUViewport(renderPass_, &viewport);
            const float viewSize[2] = {
                static_cast<float>(swapchainWidth_),
                static_cast<float>(swapchainHeight_)
            };
            SDL_PushGPUVertexUniformData(commandBuffer_, 0, viewSize, sizeof(viewSize));
            SDL_GPUBufferBinding vertexBufferBinding = {};
            vertexBufferBinding.buffer = vertexBuffer_;
            vertexBufferBinding.offset = 0;
            SDL_BindGPUVertexBuffers(renderPass_, 0, &vertexBufferBinding, 1);
            for (const DrawRun &run: worldRuns) {
                SDL_GPUTextureSamplerBinding textureBinding = {};
                textureBinding.texture = run.texture;
                textureBinding.sampler = gpuContext_->GetNearestSampler();
                SDL_BindGPUFragmentSamplers(renderPass_, 0, &textureBinding, 1);
                SDL_DrawGPUPrimitives(renderPass_, run.vertexCount, 1, run.firstVertex, 0);
            }
            if (lightingReady) {
                DrawLightingQuad();
                if (!overlayRuns.empty()) {
                    //Restore the sprite pipeline state for the overlay batch.
                    //为覆盖层批次恢复精灵管线状态。
                    SDL_BindGPUGraphicsPipeline(renderPass_, spritePipeline);
                    SDL_PushGPUVertexUniformData(commandBuffer_, 0, viewSize, sizeof(viewSize));
                    SDL_BindGPUVertexBuffers(renderPass_, 0, &vertexBufferBinding, 1);
                    for (const DrawRun &run: overlayRuns) {
                        SDL_GPUTextureSamplerBinding textureBinding = {};
                        textureBinding.texture = run.texture;
                        textureBinding.sampler = gpuContext_->GetNearestSampler();
                        SDL_BindGPUFragmentSamplers(renderPass_, 0, &textureBinding, 1);
                        SDL_DrawGPUPrimitives(renderPass_, run.vertexCount, 1, run.firstVertex, 0);
                    }
                }
            } else {
                for (const DrawRun &run: overlayRuns) {
                    SDL_GPUTextureSamplerBinding textureBinding = {};
                    textureBinding.texture = run.texture;
                    textureBinding.sampler = gpuContext_->GetNearestSampler();
                    SDL_BindGPUFragmentSamplers(renderPass_, 0, &textureBinding, 1);
                    SDL_DrawGPUPrimitives(renderPass_, run.vertexCount, 1, run.firstVertex, 0);
                }
            }
        }
    }
    //Safe to release right after the copy pass is enqueued; SDL defers
    //destruction until the command buffer completes.
    //拷贝通道入队后即可安全释放；SDL 会将销毁延迟到命令缓冲完成。
    SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
    EndActivePass();
}

void glimmer::GpuRenderer::CompositeToSwapchain() {
    if (!frameActive_) {
        return;
    }
    EndActivePass();
    //Pass 1: composite layer = game layer (game shader) + ui layer (ui shader).
    //通道 1：合成层 = game 层（game 着色器）+ ui 层（ui 着色器）。
    currentTarget_ = compositeLayerTexture_->GetGpuTexture();
    EnsureRenderPass(true);
    if (renderPass_ != nullptr) {
        //The layer pipelines are compiled lazily on the first composite pass.
        //分层管线在首次合成通道时惰性编译。
        DrawLayerQuad(GetGamePipeline(), gameLayerTexture_.get());
        DrawLayerQuad(GetUiPipeline(), uiLayerTexture_.get());
    }
    EndActivePass();
    //Pass 2: swapchain = composite layer (global shader).
    //通道 2：交换链 = 合成层（global 着色器）。
    currentTarget_ = swapchainTexture_;
    EnsureRenderPass(true);
    if (renderPass_ != nullptr) {
        DrawLayerQuad(GetGlobalPipeline(), compositeLayerTexture_.get());
    }
    EndActivePass();
    currentTarget_ = nullptr;
}

glimmer::GpuTexture *glimmer::GpuRenderer::GetUiTargetTexture() const {
    return uiLayerTexture_.get();
}

bool glimmer::GpuRenderer::SubmitFrame() {
    if (commandBuffer_ == nullptr) {
        return false;
    }
    const bool result = SDL_SubmitGPUCommandBuffer(commandBuffer_);
    if (!result) {
        LogCat::w(std::source_location::current(), "SDL_SubmitGPUCommandBuffer failed: ", SDL_GetError());
    }
    commandBuffer_ = nullptr;
    swapchainTexture_ = nullptr;
    frameActive_ = false;
    return result;
}

SDL_GPUCommandBuffer *glimmer::GpuRenderer::GetCommandBuffer() const {
    return commandBuffer_;
}

SDL_GPUTexture *glimmer::GpuRenderer::GetSwapchainTexture() const {
    return swapchainTexture_;
}

Uint32 glimmer::GpuRenderer::GetSwapchainWidth() const {
    return swapchainWidth_;
}

Uint32 glimmer::GpuRenderer::GetSwapchainHeight() const {
    return swapchainHeight_;
}

void glimmer::GpuRenderer::SetClearColor(const SDL_Color color) {
    clearColor_ = color;
}

SDL_Color glimmer::GpuRenderer::GetClearColor() const {
    return clearColor_;
}
