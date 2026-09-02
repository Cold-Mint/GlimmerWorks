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
#include "core/app/AppRenderer.h"

#include <cstddef>
#include <cstring>
#include <memory>
#include <vector>

#include "core/config/Constants.h"
#include "core/context/WindowContext.h"
#include "core/gpu/BlendMode.h"
#include "core/gpu/GpuShaderCompiler.h"
#include "core/gpu/GpuShaderCompileResult.h"
#include "core/log/LogCat.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/ResourceRef.h"
#include "core/mod/resourcePack/GPUPipelineResource.h"
#include "core/mod/resourcePack/ShaderResourceResult.h"
#include "core/scene/SceneManager.h"


glimmer::AppRenderer::AppRenderer(AppContext *appContext) : appContext_(appContext) {
    if (appContext_ == nullptr) {
        return;
    }
    WindowContext *windowContext = appContext_->GetWindowContext();
    if (windowContext != nullptr) {
        device_ = windowContext->GetDevice();
    }
    resourceLocator_ = appContext_->GetResourceLocator();
    EnsureGpuResources();
    EnsureSpritePipeline();
}

bool glimmer::AppRenderer::EnsureGpuResources() {
    if (device_ == nullptr) {
        return false;
    }
    if (sampler_ == nullptr) {
        SDL_GPUSamplerCreateInfo samplerInfo = {};
        samplerInfo.min_filter = SDL_GPU_FILTER_NEAREST;
        samplerInfo.mag_filter = SDL_GPU_FILTER_NEAREST;
        samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
        samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        sampler_ = SDL_CreateGPUSampler(device_, &samplerInfo);
        if (sampler_ == nullptr) {
            LogCat::w(std::source_location::current(), "SDL_CreateGPUSampler failed: ", SDL_GetError());
            return false;
        }
    }
    if (whiteTexture_ == nullptr) {
        SDL_GPUTextureCreateInfo textureInfo = {};
        textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
        textureInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        textureInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
        textureInfo.width = 1;
        textureInfo.height = 1;
        textureInfo.layer_count_or_depth = 1;
        textureInfo.num_levels = 1;
        textureInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;
        whiteTexture_ = SDL_CreateGPUTexture(device_, &textureInfo);
        if (whiteTexture_ == nullptr) {
            LogCat::w(std::source_location::current(), "SDL_CreateGPUTexture failed: ", SDL_GetError());
            return false;
        }

        const Uint8 whitePixel[4] = {255, 255, 255, 255};
        SDL_GPUTransferBufferCreateInfo transferInfo = {};
        transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transferInfo.size = sizeof(whitePixel);
        SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(device_, &transferInfo);
        if (transferBuffer == nullptr) {
            LogCat::w(std::source_location::current(), "SDL_CreateGPUTransferBuffer failed: ", SDL_GetError());
            return false;
        }
        void *mapped = SDL_MapGPUTransferBuffer(device_, transferBuffer, false);
        if (mapped != nullptr) {
            std::memcpy(mapped, whitePixel, sizeof(whitePixel));
            SDL_UnmapGPUTransferBuffer(device_, transferBuffer);
        }
        SDL_GPUCommandBuffer *uploadCommandBuffer = SDL_AcquireGPUCommandBuffer(device_);
        if (uploadCommandBuffer == nullptr) {
            SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
            return false;
        }
        SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(uploadCommandBuffer);
        if (copyPass == nullptr) {
            SDL_CancelGPUCommandBuffer(uploadCommandBuffer);
            SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
            return false;
        }
        SDL_GPUTextureTransferInfo source = {};
        source.transfer_buffer = transferBuffer;
        source.offset = 0;
        source.pixels_per_row = 1;
        source.rows_per_layer = 1;
        SDL_GPUTextureRegion destination = {};
        destination.texture = whiteTexture_;
        destination.mip_level = 0;
        destination.layer = 0;
        destination.w = 1;
        destination.h = 1;
        destination.d = 1;
        SDL_UploadToGPUTexture(copyPass, &source, &destination, false);
        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(uploadCommandBuffer);
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
    }
    return true;
}

SDL_GPUShader *glimmer::AppRenderer::CompileShader(const std::string &source, const bool vertex) const {
    const std::unique_ptr<GpuShaderCompileResult> result = GpuShaderCompiler::CompileToSpirv(source, vertex);
    if (result == nullptr) {
        LogCat::w(std::source_location::current(), "Failed to compile shader");
        return nullptr;
    }
    SDL_GPUShaderCreateInfo info = {};
    info.entrypoint = SHADER_ENTRY_POINT.c_str();
    info.format = SDL_GPU_SHADERFORMAT_SPIRV;
    info.stage = vertex ? SDL_GPU_SHADERSTAGE_VERTEX : SDL_GPU_SHADERSTAGE_FRAGMENT;
    info.num_samplers = result->GetNumSamplers();
    info.num_uniform_buffers = result->GetNumUniformBuffers();
    info.num_storage_textures = 0;
    info.num_storage_buffers = 0;
    info.props = 0;
    info.code = reinterpret_cast<const Uint8 *>(result->GetCode().data());
    info.code_size = result->GetCodeSize();
    SDL_GPUShader *shader = SDL_CreateGPUShader(device_, &info);
    if (shader == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_CreateGPUShader failed: ", SDL_GetError());
    }
    return shader;
}

bool glimmer::AppRenderer::EnsureSpritePipeline() {
    if (spritePipeline_ != nullptr) {
        return true;
    }
    if (device_ == nullptr) {
        return false;
    }

    SDL_GPUShader *vertexShader = nullptr;
    SDL_GPUShader *fragmentShader = nullptr;
    bool ownsVertexShader = false;
    bool ownsFragmentShader = false;
    SDL_GPUColorTargetBlendState blendState = ToColorTargetBlendState(BlendMode::Alpha);

    if (resourceLocator_ != nullptr) {
        ResourceRef pipelineRef;
        pipelineRef.SetSelfPackageId(RESOURCE_REF_CORE);
        pipelineRef.SetResourceType(RESOURCE_PIPELINE);
        pipelineRef.SetResourceKey(PIPELINE_NAME_SPRITE);
        const std::shared_ptr<GPUPipelineResourceResult> pipelineResult = resourceLocator_->FindPipeline(
            &pipelineRef, false);
        if (pipelineResult != nullptr) {
            const GPUPipelineResource *pipelineResource = pipelineResult->GetResource();
            if (pipelineResource != nullptr) {
                const std::shared_ptr<ShaderResourceResult> vertexResult = resourceLocator_->FindShader(
                    &pipelineResource->vertexShader, false);
                const std::shared_ptr<ShaderResourceResult> fragmentResult = resourceLocator_->FindShader(
                    &pipelineResource->fragmentShader, false);
                if (vertexResult != nullptr && fragmentResult != nullptr) {
                    vertexShader = vertexResult->GetResource();
                    fragmentShader = fragmentResult->GetResource();
                    blendState = ToColorTargetBlendState(BlendModeFromUint8(pipelineResource->blendMode));
                }
            }
        }
    }

    if (vertexShader == nullptr) {
        vertexShader = CompileShader(DEFAULT_SPRITE_VERT, true);
        ownsVertexShader = true;
    }
    if (fragmentShader == nullptr) {
        fragmentShader = CompileShader(DEFAULT_PASSTHROUGH_FRAG, false);
        ownsFragmentShader = true;
    }
    if (vertexShader == nullptr || fragmentShader == nullptr) {
        if (ownsVertexShader && vertexShader != nullptr) {
            SDL_ReleaseGPUShader(device_, vertexShader);
        }
        if (ownsFragmentShader && fragmentShader != nullptr) {
            SDL_ReleaseGPUShader(device_, fragmentShader);
        }
        return false;
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
    const WindowContext *windowContext = appContext_->GetWindowContext();
    SDL_Window *window = windowContext != nullptr ? windowContext->GetWindow() : nullptr;
    if (window == nullptr) {
        if (ownsVertexShader && vertexShader != nullptr) {
            SDL_ReleaseGPUShader(device_, vertexShader);
        }
        if (ownsFragmentShader && fragmentShader != nullptr) {
            SDL_ReleaseGPUShader(device_, fragmentShader);
        }
        return false;
    }
    colorTarget.format = SDL_GetGPUSwapchainTextureFormat(device_, window);
    colorTarget.blend_state = blendState;

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

    spritePipeline_ = SDL_CreateGPUGraphicsPipeline(device_, &createInfo);
    if (spritePipeline_ == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_CreateGPUGraphicsPipeline failed: ", SDL_GetError());
    }

    if (ownsVertexShader && vertexShader != nullptr) {
        SDL_ReleaseGPUShader(device_, vertexShader);
    }
    if (ownsFragmentShader && fragmentShader != nullptr) {
        SDL_ReleaseGPUShader(device_, fragmentShader);
    }
    return spritePipeline_ != nullptr;
}

void glimmer::AppRenderer::EnsureVertexBufferSize(const Uint32 size) {
    if (size <= vertexBufferSize_) {
        return;
    }
    Uint32 newSize = vertexBufferSize_ > 0 ? vertexBufferSize_ : 1;
    while (newSize < size) {
        newSize *= 2;
    }
    if (vertexBuffer_ != nullptr) {
        SDL_ReleaseGPUBuffer(device_, vertexBuffer_);
    }
    SDL_GPUBufferCreateInfo info = {};
    info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    info.size = newSize;
    vertexBuffer_ = SDL_CreateGPUBuffer(device_, &info);
    vertexBufferSize_ = vertexBuffer_ != nullptr ? newSize : 0;
}

void glimmer::AppRenderer::EnsureIndexBufferSize(const Uint32 size) {
    if (size <= indexBufferSize_) {
        return;
    }
    Uint32 newSize = indexBufferSize_ > 0 ? indexBufferSize_ : 1;
    while (newSize < size) {
        newSize *= 2;
    }
    if (indexBuffer_ != nullptr) {
        SDL_ReleaseGPUBuffer(device_, indexBuffer_);
    }
    SDL_GPUBufferCreateInfo info = {};
    info.usage = SDL_GPU_BUFFERUSAGE_INDEX;
    info.size = newSize;
    indexBuffer_ = SDL_CreateGPUBuffer(device_, &info);
    indexBufferSize_ = indexBuffer_ != nullptr ? newSize : 0;
}

void glimmer::AppRenderer::EnsureTransferBufferSize(const Uint32 size) {
    if (size <= transferBufferSize_) {
        return;
    }
    Uint32 newSize = transferBufferSize_ > 0 ? transferBufferSize_ : 1;
    while (newSize < size) {
        newSize *= 2;
    }
    if (transferBuffer_ != nullptr) {
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer_);
    }
    SDL_GPUTransferBufferCreateInfo info = {};
    info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    info.size = newSize;
    transferBuffer_ = SDL_CreateGPUTransferBuffer(device_, &info);
    transferBufferSize_ = transferBuffer_ != nullptr ? newSize : 0;
}

void glimmer::AppRenderer::FlushQueue(SDL_GPUCommandBuffer *commandBuffer, SDL_GPUTexture *swapchainTexture,
                                      const Uint32 width, const Uint32 height) {
    const bool hasCommands = !renderQueue_.IsEmpty();

    if (hasCommands) {
        renderQueue_.Sort();
        const std::vector<RenderCommand> &commands = renderQueue_.GetCommands();

        std::vector<SpriteVertex> vertices;
        std::vector<Uint32> indices;
        vertices.reserve(commands.size() * 4);
        indices.reserve(commands.size() * 6);
        for (const RenderCommand &command : commands) {
            const Uint32 baseIndex = static_cast<Uint32>(vertices.size());
            vertices.insert(vertices.end(), command.corners, command.corners + 4);
            const Uint32 quadIndices[6] = {
                baseIndex + 0, baseIndex + 1, baseIndex + 2,
                baseIndex + 1, baseIndex + 3, baseIndex + 2
            };
            indices.insert(indices.end(), quadIndices, quadIndices + 6);
        }

        const Uint32 vertexDataSize = static_cast<Uint32>(vertices.size() * sizeof(SpriteVertex));
        const Uint32 indexDataSize = static_cast<Uint32>(indices.size() * sizeof(Uint32));
        EnsureVertexBufferSize(vertexDataSize);
        EnsureIndexBufferSize(indexDataSize);
        EnsureTransferBufferSize(vertexDataSize + indexDataSize);

        if (vertexBuffer_ != nullptr && indexBuffer_ != nullptr && transferBuffer_ != nullptr) {
            void *mapped = SDL_MapGPUTransferBuffer(device_, transferBuffer_, true);
            if (mapped != nullptr) {
                std::memcpy(static_cast<Uint8 *>(mapped), vertices.data(), vertexDataSize);
                std::memcpy(static_cast<Uint8 *>(mapped) + vertexDataSize, indices.data(), indexDataSize);
                SDL_UnmapGPUTransferBuffer(device_, transferBuffer_);
            }

            SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);
            if (copyPass != nullptr) {
                SDL_GPUTransferBufferLocation vertexSource = {transferBuffer_, 0};
                SDL_GPUBufferRegion vertexDestination = {vertexBuffer_, 0, vertexDataSize};
                SDL_UploadToGPUBuffer(copyPass, &vertexSource, &vertexDestination, true);
                SDL_GPUTransferBufferLocation indexSource = {transferBuffer_, vertexDataSize};
                SDL_GPUBufferRegion indexDestination = {indexBuffer_, 0, indexDataSize};
                SDL_UploadToGPUBuffer(copyPass, &indexSource, &indexDestination, true);
                SDL_EndGPUCopyPass(copyPass);
            }
        }
    }

    SDL_GPUColorTargetInfo colorTarget = {};
    colorTarget.texture = swapchainTexture;
    colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;
    colorTarget.clear_color = {0.0F, 0.0F, 0.0F, 1.0F};

    SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTarget, 1, nullptr);
    if (renderPass == nullptr) {
        return;
    }

    if (hasCommands && spritePipeline_ != nullptr && vertexBuffer_ != nullptr && indexBuffer_ != nullptr &&
        sampler_ != nullptr) {
        SDL_BindGPUGraphicsPipeline(renderPass, spritePipeline_);

        SDL_GPUBufferBinding vertexBinding = {vertexBuffer_, 0};
        SDL_BindGPUVertexBuffers(renderPass, 0, &vertexBinding, 1);
        SDL_GPUBufferBinding indexBinding = {indexBuffer_, 0};
        SDL_BindGPUIndexBuffer(renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

        const float viewSize[2] = {static_cast<float>(width), static_cast<float>(height)};
        SDL_PushGPUVertexUniformData(commandBuffer, 0, viewSize, sizeof(viewSize));

        const std::vector<RenderCommand> &commands = renderQueue_.GetCommands();
        Uint32 firstIndex = 0;
        for (const RenderCommand &command : commands) {
            SDL_GPUTexture *texture = whiteTexture_;
            if (command.texture != nullptr && command.texture->GetResource() != nullptr) {
                texture = command.texture->GetResource();
            }
            if (texture == nullptr) {
                firstIndex += 6;
                continue;
            }
            SDL_GPUTextureSamplerBinding textureSamplerBinding = {texture, sampler_};
            SDL_BindGPUFragmentSamplers(renderPass, 0, &textureSamplerBinding, 1);
            SDL_DrawGPUIndexedPrimitives(renderPass, 6, 1, firstIndex, 0, 0);
            firstIndex += 6;
        }
    }

    SDL_EndGPURenderPass(renderPass);
}

void glimmer::AppRenderer::RenderFrame(const RmlContext *rmlContext, const int windowWidth, const int windowHeight,
                                       const uint64_t frameStart, const float deltaTime) {
    (void) frameStart;
    (void) deltaTime;
    if (windowWidth <= 0 || windowHeight <= 0) {
        return;
    }
    if (device_ == nullptr) {
        return;
    }
    if (!EnsureSpritePipeline()) {
        return;
    }
    WindowContext *windowContext = appContext_->GetWindowContext();
    if (windowContext == nullptr) {
        return;
    }
    SDL_Window *window = windowContext->GetWindow();
    if (window == nullptr) {
        return;
    }

    //Collect this frame's render commands, then upload and draw them in one
    //sorted batch before compositing RmlUi on top.
    //收集本帧的渲染命令，然后以排好序的批次上传并绘制，最后把 RmlUi 合成在上层。
    renderQueue_.Clear();
    RenderScenes();
    RenderOverlays();

    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(device_);
    if (commandBuffer == nullptr) {
        return;
    }
    SDL_GPUTexture *swapchainTexture = nullptr;
    Uint32 swapchainWidth = 0;
    Uint32 swapchainHeight = 0;
    if (!SDL_AcquireGPUSwapchainTexture(commandBuffer, window, &swapchainTexture, &swapchainWidth, &swapchainHeight)) {
        LogCat::w(std::source_location::current(), "SDL_AcquireGPUSwapchainTexture failed: ", SDL_GetError());
        SDL_CancelGPUCommandBuffer(commandBuffer);
        return;
    }

    // The projection uniform maps scene geometry (which is authored in logical
    // window coordinates) into NDC, so it must use the logical window size
    // rather than the physical swapchain dimensions.
    // 投影 uniform 把场景几何（以逻辑窗口坐标书写）映射到 NDC，因此必须使用
    // 逻辑窗口尺寸而非物理交换链尺寸。
    FlushQueue(commandBuffer, swapchainTexture, static_cast<Uint32>(windowWidth), static_cast<Uint32>(windowHeight));

    if (rmlContext != nullptr) {
        rmlContext->RenderContext(commandBuffer, swapchainTexture, swapchainWidth, swapchainHeight);
    }

    if (!SDL_SubmitGPUCommandBuffer(commandBuffer)) {
        LogCat::w(std::source_location::current(), "SDL_SubmitGPUCommandBuffer failed: ", SDL_GetError());
    }
}

void glimmer::AppRenderer::RenderScenes() {
    SceneManager *sceneManager = appContext_->GetSceneManager();
    if (Scene *topScene = sceneManager->GetTopScene(); topScene != nullptr) {
        topScene->Render(&renderQueue_);
    }
}

void glimmer::AppRenderer::RenderOverlays() {
    SceneManager *sceneManager = appContext_->GetSceneManager();
    const auto &overlayScenes = sceneManager->GetOverlayScenes();
    for (const auto overlay: overlayScenes) {
        overlay->Render(&renderQueue_);
    }
}
