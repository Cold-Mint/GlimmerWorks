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
#include "SpriteRenderer.h"

#include <cmath>

#include "GpuShaderCompiler.h"
#include "core/Config.h"
#include "core/Constants.h"
#include "core/log/LogCat.h"
#include "core/mod/resourcePack/ResourcePackManager.h"

namespace {
    //Shader names inside the resource packs (directory shaders/@core/).
    //材质包内的着色器名（目录 shaders/@core/）。
    constexpr const char *SHADER_NAME_SPRITE_VERT = "sprite";
    constexpr const char *SHADER_NAME_SPRITE_FRAG = "sprite";
    constexpr const char *SHADER_NAME_GAME_FRAG = "game";
    constexpr const char *SHADER_NAME_UI_FRAG = "ui";
    constexpr const char *SHADER_NAME_GLOBAL_FRAG = "global";
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
}

glimmer::SpriteRenderer::~SpriteRenderer() {
    Shutdown();
}

SDL_GPUGraphicsPipeline *glimmer::SpriteRenderer::CreateSpritePipeline(SDL_GPUShader *vertexShader,
                                                                       SDL_GPUShader *fragmentShader,
                                                                       const bool enableBlend) const {
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

    //Standard alpha blending, equivalent to SDL_BLENDMODE_BLEND.
    //标准 alpha 混合，等价于 SDL_BLENDMODE_BLEND。
    SDL_GPUColorTargetDescription colorTarget = {};
    colorTarget.format = gpuContext_->GetSwapchainFormat();
    colorTarget.blend_state.enable_blend = enableBlend;
    colorTarget.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTarget.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTarget.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTarget.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTarget.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTarget.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

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

bool glimmer::SpriteRenderer::Init(GpuContext *gpuContext, ResourcePackManager *resourcePackManager,
                                   const Mods &mods) {
    if (gpuContext == nullptr || gpuContext->GetDevice() == nullptr) {
        LogCat::w(std::source_location::current(), "gpuContext is nullptr or not initialized");
        return false;
    }
    if (resourcePackManager == nullptr) {
        LogCat::w(std::source_location::current(), "resourcePackManager is nullptr");
        return false;
    }
    gpuContext_ = gpuContext;
    device_ = gpuContext->GetDevice();

    //Load shader sources from the enabled resource packs; fall back to the
    //embedded pass-through shaders when a pack does not provide them.
    //从已启用的材质包加载着色器源码；材质包未提供时使用内嵌的 pass-through 兜底。
    auto loadShader = [&](const std::string &key, const std::string &extension, const SDL_GPUShaderStage stage,
                          const Uint32 numSamplers, const Uint32 numUniformBuffers,
                          const char *fallbackSource) -> SDL_GPUShader * {
        auto source = resourcePackManager->LoadShaderSource(RESOURCE_REF_CORE, key, extension, mods);
        if (source.has_value()) {
            SDL_GPUShader *shader = GpuShaderCompiler::CompileFromSource(
                device_, source->c_str(), (key + "." + extension).c_str(), stage, numSamplers, numUniformBuffers);
            if (shader != nullptr) {
                return shader;
            }
            LogCat::w(std::source_location::current(), "Pack shader failed to compile, using fallback: ", key);
        }
        return GpuShaderCompiler::CompileFromSource(device_, fallbackSource, (key + "(fallback)").c_str(), stage,
                                                    numSamplers, numUniformBuffers);
    };

    SDL_GPUShader *vertexShader = loadShader(SHADER_NAME_SPRITE_VERT, SHADER_EXTENSION_VERT,
                                             SDL_GPU_SHADERSTAGE_VERTEX, 0, 1, DEFAULT_SPRITE_VERT);
    SDL_GPUShader *spriteFragShader = loadShader(SHADER_NAME_SPRITE_FRAG, SHADER_EXTENSION_FRAG,
                                                 SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 0, DEFAULT_PASSTHROUGH_FRAG);
    SDL_GPUShader *gameFragShader = loadShader(SHADER_NAME_GAME_FRAG, SHADER_EXTENSION_FRAG,
                                               SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 0, DEFAULT_PASSTHROUGH_FRAG);
    SDL_GPUShader *uiFragShader = loadShader(SHADER_NAME_UI_FRAG, SHADER_EXTENSION_FRAG,
                                             SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 0, DEFAULT_PASSTHROUGH_FRAG);
    SDL_GPUShader *globalFragShader = loadShader(SHADER_NAME_GLOBAL_FRAG, SHADER_EXTENSION_FRAG,
                                                 SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 0, DEFAULT_PASSTHROUGH_FRAG);
    if (vertexShader == nullptr || spriteFragShader == nullptr || gameFragShader == nullptr ||
        uiFragShader == nullptr || globalFragShader == nullptr) {
        LogCat::e(std::source_location::current(), "Failed to compile sprite shaders");
        if (vertexShader != nullptr) SDL_ReleaseGPUShader(device_, vertexShader);
        if (spriteFragShader != nullptr) SDL_ReleaseGPUShader(device_, spriteFragShader);
        if (gameFragShader != nullptr) SDL_ReleaseGPUShader(device_, gameFragShader);
        if (uiFragShader != nullptr) SDL_ReleaseGPUShader(device_, uiFragShader);
        if (globalFragShader != nullptr) SDL_ReleaseGPUShader(device_, globalFragShader);
        return false;
    }

    pipeline_ = CreateSpritePipeline(vertexShader, spriteFragShader, true);
    gamePipeline_ = CreateSpritePipeline(vertexShader, gameFragShader, true);
    uiPipeline_ = CreateSpritePipeline(vertexShader, uiFragShader, true);
    globalPipeline_ = CreateSpritePipeline(vertexShader, globalFragShader, false);
    SDL_ReleaseGPUShader(device_, vertexShader);
    SDL_ReleaseGPUShader(device_, spriteFragShader);
    SDL_ReleaseGPUShader(device_, gameFragShader);
    SDL_ReleaseGPUShader(device_, uiFragShader);
    SDL_ReleaseGPUShader(device_, globalFragShader);
    if (pipeline_ == nullptr || gamePipeline_ == nullptr || uiPipeline_ == nullptr || globalPipeline_ == nullptr) {
        LogCat::e(std::source_location::current(), "Failed to create sprite pipelines");
        Shutdown();
        return false;
    }

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
    LogCat::i("SpriteRenderer initialized");
    return true;
}

void glimmer::SpriteRenderer::Shutdown() {
    if (device_ != nullptr) {
        whiteTexture_.reset();
        compositeLayerTexture_.reset();
        uiLayerTexture_.reset();
        gameLayerTexture_.reset();
        if (unitQuadBuffer_ != nullptr) {
            SDL_ReleaseGPUBuffer(device_, unitQuadBuffer_);
            unitQuadBuffer_ = nullptr;
        }
        if (vertexBuffer_ != nullptr) {
            SDL_ReleaseGPUBuffer(device_, vertexBuffer_);
            vertexBuffer_ = nullptr;
        }
        vertexBufferCapacity_ = 0;
        if (globalPipeline_ != nullptr) {
            SDL_ReleaseGPUGraphicsPipeline(device_, globalPipeline_);
            globalPipeline_ = nullptr;
        }
        if (uiPipeline_ != nullptr) {
            SDL_ReleaseGPUGraphicsPipeline(device_, uiPipeline_);
            uiPipeline_ = nullptr;
        }
        if (gamePipeline_ != nullptr) {
            SDL_ReleaseGPUGraphicsPipeline(device_, gamePipeline_);
            gamePipeline_ = nullptr;
        }
        if (pipeline_ != nullptr) {
            SDL_ReleaseGPUGraphicsPipeline(device_, pipeline_);
            pipeline_ = nullptr;
        }
    }
    device_ = nullptr;
    gpuContext_ = nullptr;
}

bool glimmer::SpriteRenderer::EnsureLayerTextures() {
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

bool glimmer::SpriteRenderer::BeginFrame(SDL_Window *window) {
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
    vertices_.clear();
    runs_.clear();
    currentTexture_ = nullptr;
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

void glimmer::SpriteRenderer::EnsureRenderPass(const bool clear) {
    if (renderPass_ != nullptr || !frameActive_ || currentTarget_ == nullptr) {
        return;
    }
    SDL_GPUColorTargetInfo colorTargetInfo = {};
    colorTargetInfo.texture = currentTarget_;
    colorTargetInfo.mip_level = 0;
    colorTargetInfo.layer_or_depth_plane = 0;
    colorTargetInfo.load_op = clear ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
    colorTargetInfo.clear_color = {
        static_cast<float>(drawColor_.r) / 255.0F,
        static_cast<float>(drawColor_.g) / 255.0F,
        static_cast<float>(drawColor_.b) / 255.0F,
        static_cast<float>(drawColor_.a) / 255.0F
    };
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.cycle = false;
    renderPass_ = SDL_BeginGPURenderPass(commandBuffer_, &colorTargetInfo, 1, nullptr);
    if (renderPass_ == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_BeginGPURenderPass failed: ", SDL_GetError());
    }
}

void glimmer::SpriteRenderer::EndActivePass() {
    if (renderPass_ != nullptr) {
        SDL_EndGPURenderPass(renderPass_);
        renderPass_ = nullptr;
    }
}

void glimmer::SpriteRenderer::DrawLayerQuad(SDL_GPUGraphicsPipeline *pipeline, const GpuTexture *source) {
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

void glimmer::SpriteRenderer::CompositeToSwapchain() {
    if (!frameActive_) {
        return;
    }
    EndActivePass();
    //Pass 1: composite layer = game layer (game shader) + ui layer (ui shader).
    //通道 1：合成层 = game 层（game 着色器）+ ui 层（ui 着色器）。
    currentTarget_ = compositeLayerTexture_->GetGpuTexture();
    EnsureRenderPass(true);
    if (renderPass_ != nullptr) {
        DrawLayerQuad(gamePipeline_, gameLayerTexture_.get());
        DrawLayerQuad(uiPipeline_, uiLayerTexture_.get());
    }
    EndActivePass();
    //Pass 2: swapchain = composite layer (global shader).
    //通道 2：交换链 = 合成层（global 着色器）。
    currentTarget_ = swapchainTexture_;
    EnsureRenderPass(true);
    if (renderPass_ != nullptr) {
        DrawLayerQuad(globalPipeline_, compositeLayerTexture_.get());
    }
    EndActivePass();
    currentTarget_ = nullptr;
}

glimmer::GpuTexture *glimmer::SpriteRenderer::GetUiTargetTexture() const {
    return uiLayerTexture_.get();
}

void glimmer::SpriteRenderer::EndFrame() {
    if (!frameActive_) {
        return;
    }
    //Upload and draw any pending geometry.
    //上传并绘制所有待处理的几何数据。
    if (!vertices_.empty()) {
        EndActivePass();
        const Uint32 dataSize = static_cast<Uint32>(vertices_.size() * sizeof(SpriteVertex));
        if (vertices_.size() > vertexBufferCapacity_) {
            Uint32 newCapacity = vertexBufferCapacity_ == 0 ? INITIAL_VERTEX_CAPACITY : vertexBufferCapacity_;
            while (newCapacity < vertices_.size()) {
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
        if (vertexBuffer_ != nullptr) {
            SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {};
            transferBufferCreateInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
            transferBufferCreateInfo.size = dataSize;
            transferBufferCreateInfo.props = 0;
            SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(device_, &transferBufferCreateInfo);
            if (transferBuffer != nullptr) {
                void *mapped = SDL_MapGPUTransferBuffer(device_, transferBuffer, false);
                if (mapped != nullptr) {
                    SDL_memcpy(mapped, vertices_.data(), dataSize);
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
                        if (renderPass_ != nullptr) {
                            SDL_BindGPUGraphicsPipeline(renderPass_, pipeline_);
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
                            for (const DrawRun &run: runs_) {
                                SDL_GPUTextureSamplerBinding textureBinding = {};
                                textureBinding.texture = run.texture;
                                textureBinding.sampler = gpuContext_->GetNearestSampler();
                                SDL_BindGPUFragmentSamplers(renderPass_, 0, &textureBinding, 1);
                                SDL_DrawGPUPrimitives(renderPass_, run.vertexCount, 1, run.firstVertex, 0);
                            }
                        }
                    }
                }
                //Safe to release right after the copy pass is enqueued; SDL
                //defers destruction until the command buffer completes.
                //拷贝通道入队后即可安全释放；SDL 会将销毁延迟到命令缓冲完成。
                SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
            }
        }
        vertices_.clear();
        runs_.clear();
        currentTexture_ = nullptr;
    }
    EndActivePass();
}

bool glimmer::SpriteRenderer::SubmitFrame() {
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

SDL_GPUCommandBuffer *glimmer::SpriteRenderer::GetCommandBuffer() const {
    return commandBuffer_;
}

SDL_GPUTexture *glimmer::SpriteRenderer::GetSwapchainTexture() const {
    return swapchainTexture_;
}

Uint32 glimmer::SpriteRenderer::GetSwapchainWidth() const {
    return swapchainWidth_;
}

Uint32 glimmer::SpriteRenderer::GetSwapchainHeight() const {
    return swapchainHeight_;
}

void glimmer::SpriteRenderer::SetDrawColor(const SDL_Color color) {
    drawColor_ = color;
}

SDL_Color glimmer::SpriteRenderer::GetDrawColor() const {
    return drawColor_;
}

void glimmer::SpriteRenderer::AppendQuad(SDL_GPUTexture *texture, const SDL_FPoint positions[4],
                                         const SDL_FPoint uvs[4], const SDL_Color &color) {
    if (!frameActive_ || renderPass_ == nullptr) {
        return;
    }
    if (vertices_.size() + 6 > vertexBufferCapacity_) {
        //Flush pending geometry before growing the CPU batch.
        //在扩充 CPU 批次之前先刷新待处理的几何数据。
        EndFrame();
        EnsureRenderPass(false);
        if (renderPass_ == nullptr) {
            return;
        }
    }
    if (currentTexture_ != texture) {
        currentTexture_ = texture;
        runs_.push_back({texture, static_cast<Uint32>(vertices_.size()), 0});
    }
    const SpriteVertex topLeft = {
        positions[0].x, positions[0].y, uvs[0].x, uvs[0].y, color.r, color.g, color.b, color.a
    };
    const SpriteVertex topRight = {
        positions[1].x, positions[1].y, uvs[1].x, uvs[1].y, color.r, color.g, color.b, color.a
    };
    const SpriteVertex bottomLeft = {
        positions[2].x, positions[2].y, uvs[2].x, uvs[2].y, color.r, color.g, color.b, color.a
    };
    const SpriteVertex bottomRight = {
        positions[3].x, positions[3].y, uvs[3].x, uvs[3].y, color.r, color.g, color.b, color.a
    };
    vertices_.push_back(topLeft);
    vertices_.push_back(topRight);
    vertices_.push_back(bottomLeft);
    vertices_.push_back(topRight);
    vertices_.push_back(bottomRight);
    vertices_.push_back(bottomLeft);
    runs_.back().vertexCount += 6;
}

void glimmer::SpriteRenderer::DrawTexture(const GpuTexture *texture, const SDL_FRect *src, const SDL_FRect *dst,
                                          const SDL_Color &mod) {
    if (texture == nullptr || !texture->IsValid()) {
        return;
    }
    const float textureWidth = static_cast<float>(texture->w);
    const float textureHeight = static_cast<float>(texture->h);
    SDL_FRect dstRect;
    if (dst == nullptr) {
        dstRect = {0.0F, 0.0F, textureWidth, textureHeight};
    } else {
        dstRect = *dst;
    }
    if (dstRect.w <= 0.0F || dstRect.h <= 0.0F) {
        return;
    }
    float u0 = 0.0F;
    float v0 = 0.0F;
    float u1 = 1.0F;
    float v1 = 1.0F;
    if (src != nullptr) {
        u0 = src->x / textureWidth;
        v0 = src->y / textureHeight;
        u1 = (src->x + src->w) / textureWidth;
        v1 = (src->y + src->h) / textureHeight;
    }
    const SDL_FPoint positions[4] = {
        {dstRect.x, dstRect.y},
        {dstRect.x + dstRect.w, dstRect.y},
        {dstRect.x, dstRect.y + dstRect.h},
        {dstRect.x + dstRect.w, dstRect.y + dstRect.h}
    };
    const SDL_FPoint uvs[4] = {
        {u0, v0},
        {u1, v0},
        {u0, v1},
        {u1, v1}
    };
    AppendQuad(texture->GetGpuTexture(), positions, uvs, mod);
}

void glimmer::SpriteRenderer::DrawTextureRotated(const GpuTexture *texture, const SDL_FRect *src,
                                                 const SDL_FRect *dst, const double angleDegrees,
                                                 const SDL_FPoint *center, const Uint8 flip, const SDL_Color &mod) {
    if (texture == nullptr || !texture->IsValid() || dst == nullptr) {
        return;
    }
    if (dst->w <= 0.0F || dst->h <= 0.0F) {
        return;
    }
    const float textureWidth = static_cast<float>(texture->w);
    const float textureHeight = static_cast<float>(texture->h);
    float u0 = 0.0F;
    float v0 = 0.0F;
    float u1 = 1.0F;
    float v1 = 1.0F;
    if (src != nullptr) {
        u0 = src->x / textureWidth;
        v0 = src->y / textureHeight;
        u1 = (src->x + src->w) / textureWidth;
        v1 = (src->y + src->h) / textureHeight;
    }
    SDL_FPoint positions[4] = {
        {dst->x, dst->y},
        {dst->x + dst->w, dst->y},
        {dst->x, dst->y + dst->h},
        {dst->x + dst->w, dst->y + dst->h}
    };
    if (angleDegrees != 0.0) {
        SDL_FPoint rotationCenter;
        if (center == nullptr) {
            rotationCenter = {dst->x + dst->w * 0.5F, dst->y + dst->h * 0.5F};
        } else {
            rotationCenter = {dst->x + center->x, dst->y + center->y};
        }
        //Positive angles rotate clockwise (SDL_RenderTextureRotated semantics,
        //+Y points down in screen space).
        //正角度顺时针旋转（SDL_RenderTextureRotated 语义，屏幕空间 +Y 向下）。
        const double radians = angleDegrees * (3.14159265358979323846 / 180.0);
        const auto cosValue = static_cast<float>(std::cos(radians));
        const auto sinValue = static_cast<float>(std::sin(radians));
        for (auto &position: positions) {
            const float dx = position.x - rotationCenter.x;
            const float dy = position.y - rotationCenter.y;
            position.x = rotationCenter.x + dx * cosValue - dy * sinValue;
            position.y = rotationCenter.y + dx * sinValue + dy * cosValue;
        }
    }
    SDL_FPoint uvs[4] = {
        {u0, v0},
        {u1, v0},
        {u0, v1},
        {u1, v1}
    };
    if ((flip & FLIP_HORIZONTAL) != 0) {
        std::swap(uvs[0].x, uvs[1].x);
        std::swap(uvs[2].x, uvs[3].x);
    }
    if ((flip & FLIP_VERTICAL) != 0) {
        std::swap(uvs[0].y, uvs[2].y);
        std::swap(uvs[1].y, uvs[3].y);
    }
    AppendQuad(texture->GetGpuTexture(), positions, uvs, mod);
}

void glimmer::SpriteRenderer::FillRect(const SDL_FRect *rect) {
    if (rect == nullptr || whiteTexture_ == nullptr) {
        return;
    }
    if (rect->w <= 0.0F || rect->h <= 0.0F) {
        return;
    }
    const SDL_FPoint positions[4] = {
        {rect->x, rect->y},
        {rect->x + rect->w, rect->y},
        {rect->x, rect->y + rect->h},
        {rect->x + rect->w, rect->y + rect->h}
    };
    const SDL_FPoint uvs[4] = {
        {0.0F, 0.0F},
        {1.0F, 0.0F},
        {0.0F, 1.0F},
        {1.0F, 1.0F}
    };
    AppendQuad(whiteTexture_->GetGpuTexture(), positions, uvs, drawColor_);
}

void glimmer::SpriteRenderer::DrawRect(const SDL_FRect *rect) {
    if (rect == nullptr) {
        return;
    }
    const SDL_FRect top = {rect->x, rect->y, rect->w, 1.0F};
    const SDL_FRect bottom = {rect->x, rect->y + rect->h - 1.0F, rect->w, 1.0F};
    const SDL_FRect left = {rect->x, rect->y + 1.0F, 1.0F, rect->h - 2.0F};
    const SDL_FRect right = {rect->x + rect->w - 1.0F, rect->y + 1.0F, 1.0F, rect->h - 2.0F};
    FillRect(&top);
    FillRect(&bottom);
    FillRect(&left);
    FillRect(&right);
}

void glimmer::SpriteRenderer::DrawLine(const float x1, const float y1, const float x2, const float y2) {
    const float dx = x2 - x1;
    const float dy = y2 - y1;
    const float length = std::sqrt(dx * dx + dy * dy);
    if (length <= 0.0F) {
        DrawPoint(x1, y1);
        return;
    }
    //A 1-pixel thick quad perpendicular to the line direction.
    //垂直于线段方向的 1 像素宽四边形。
    const float nx = -dy / length * 0.5F;
    const float ny = dx / length * 0.5F;
    const SDL_FPoint positions[4] = {
        {x1 + nx, y1 + ny},
        {x1 - nx, y1 - ny},
        {x2 + nx, y2 + ny},
        {x2 - nx, y2 - ny}
    };
    const SDL_FPoint uvs[4] = {
        {0.0F, 0.0F},
        {1.0F, 0.0F},
        {0.0F, 1.0F},
        {1.0F, 1.0F}
    };
    if (whiteTexture_ == nullptr) {
        return;
    }
    AppendQuad(whiteTexture_->GetGpuTexture(), positions, uvs, drawColor_);
}

void glimmer::SpriteRenderer::DrawPoint(const float x, const float y) {
    const SDL_FRect rect = {x, y, 1.0F, 1.0F};
    FillRect(&rect);
}
