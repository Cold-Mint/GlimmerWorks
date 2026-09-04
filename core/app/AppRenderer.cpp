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
#include "core/context/CacheContext.h"
#include "core/context/WindowContext.h"
#include "core/ecs/component/CameraComponent.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/log/LogCat.h"
#include "core/math/CoordinateTransformer.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/ResourceRef.h"
#include "core/mod/resourcePack/GPUPipelineResourceResult.h"
#include "core/scene/SceneManager.h"
#include "core/scene/WorldScene.h"
#include "core/world/AmbientLight.h"
#include "core/world/WorldContext.h"


glimmer::AppRenderer::AppRenderer(AppContext *appContext) : appContext_(appContext) {
    if (appContext_ == nullptr) {
        return;
    }
    WindowContext *windowContext = appContext_->GetWindowContext();
    if (windowContext != nullptr) {
        device_ = windowContext->GetDevice();
    }
    resourceLocator_ = appContext_->GetResourceLocator();
    ResourceRef defaultPipelineResourceRef;
    defaultPipelineResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    defaultPipelineResourceRef.SetResourceType(RESOURCE_PIPELINE);
    defaultPipelineResourceRef.SetResourceKey("default");
    defaultPipeline_ = resourceLocator_->FindGPUGraphicsPipeline(&defaultPipelineResourceRef);
    if (defaultPipeline_ == nullptr) {
        LogCat::e(std::source_location::current(), "defaultPipeline failed: ", SDL_GetError());
    }
    ResourceRef defaultSamplerResourceRef;
    defaultSamplerResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    defaultSamplerResourceRef.SetResourceType(RESOURCE_SAMPLER);
    defaultSamplerResourceRef.SetResourceKey("default");
    defaultSampler_ = resourceLocator_->FindGPUGraphicsSampler(&defaultSamplerResourceRef);
    if (defaultSampler_ == nullptr) {
        LogCat::e(std::source_location::current(), "defaultSampler failed: ", SDL_GetError());
    }
}

void glimmer::AppRenderer::RenderFrame(const RmlContext *rmlContext, const int windowWidth, const int windowHeight) {
    if (windowWidth <= 0 || windowHeight <= 0) {
        return;
    }
    if (device_ == nullptr) {
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

    //Collect this frame's render commands, then render the unlit scene into an
    //offscreen target and composite lighting + RmlUi on top.
    //收集本帧的渲染命令，然后把无光照场景渲染到离屏目标，再在上层合成光照与 RmlUi。
    renderQueue_.Clear();
    RenderScenes();
    RenderOverlays();

    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(device_);
    if (commandBuffer == nullptr) {
        return;
    }
    SDL_GPUTexture *swapChainTexture = nullptr;
    Uint32 swapChainWidth = 0;
    Uint32 swapChainHeight = 0;
    if (!SDL_AcquireGPUSwapchainTexture(commandBuffer, window, &swapChainTexture, &swapChainWidth, &swapChainHeight)) {
        LogCat::w(std::source_location::current(), "SDL_AcquireGPUSwapchainTexture failed: ", SDL_GetError());
        SDL_CancelGPUCommandBuffer(commandBuffer);
        return;
    }

    const auto logicalWidth = static_cast<Uint32>(windowWidth);
    const auto logicalHeight = static_cast<Uint32>(windowHeight);

    // Pass 1: render the unlit scene into the offscreen target.
    // 通道 1：将无光照场景渲染到离屏目标。
    EnsureSceneTexture(logicalWidth, logicalHeight);
    FlushScenePass(commandBuffer, sceneTexture_, logicalWidth, logicalHeight);

    // Build and upload the per-tile light map for the camera viewport.
    // 构建并上传相机视口的逐瓦片光照贴图。
    LightBuffer *lightBuffer = nullptr;
    CameraComponent *camera = nullptr;
    Transform2DComponent *cameraTransform = nullptr;
    WorldContext *worldContext = nullptr;
    if (SceneManager *sceneManager = appContext_->GetSceneManager(); sceneManager != nullptr) {
        if (auto *worldScene = dynamic_cast<WorldScene *>(sceneManager->GetTopScene()); worldScene != nullptr) {
            worldContext = worldScene->GetWorldContext();
            if (worldContext != nullptr) {
                lightBuffer = worldContext->GetLightingBuffer();
                EntityShortCut *entityShortCut = worldContext->GetEntityShortCut();
                if (entityShortCut != nullptr) {
                    camera = entityShortCut->GetCameraComponent();
                    cameraTransform = entityShortCut->GetCameraTransform2DComponent();
                }
            }
        }
    }
    UpdateLightMap(lightBuffer, camera, cameraTransform, worldContext, logicalWidth, logicalHeight);
    lightMapTexture_.Upload(commandBuffer);
    FlushLightingPass(commandBuffer, swapChainTexture);
    if (rmlContext != nullptr) {
        rmlContext->RenderContext(commandBuffer, swapChainTexture, logicalWidth, logicalHeight);
    }
    if (!SDL_SubmitGPUCommandBuffer(commandBuffer)) {
        LogCat::w(std::source_location::current(), "SDL_SubmitGPUCommandBuffer failed: ", SDL_GetError());
    }
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

void glimmer::AppRenderer::FlushScenePass(SDL_GPUCommandBuffer *commandBuffer, SDL_GPUTexture *targetTexture,
                                          const Uint32 width, const Uint32 height) {
    const bool hasCommands = !renderQueue_.IsEmpty();

    if (hasCommands) {
        renderQueue_.Sort();
        const std::vector<RenderCommand> &commands = renderQueue_.GetCommands();
        std::vector<SpriteVertex> vertices;
        std::vector<Uint32> indices;
        vertices.reserve(commands.size() * 4);
        indices.reserve(commands.size() * 6);
        for (const RenderCommand &command: commands) {
            const auto baseIndex = static_cast<Uint32>(vertices.size());
            vertices.insert(vertices.end(), command.corners, command.corners + 4);
            const Uint32 quadIndices[6] = {
                baseIndex + 0, baseIndex + 1, baseIndex + 2,
                baseIndex + 1, baseIndex + 3, baseIndex + 2
            };
            indices.insert(indices.end(), quadIndices, quadIndices + 6);
        }
        const auto vertexDataSize = static_cast<Uint32>(vertices.size() * sizeof(SpriteVertex));
        const auto indexDataSize = static_cast<Uint32>(indices.size() * sizeof(Uint32));
        EnsureVertexBufferSize(vertexDataSize);
        EnsureIndexBufferSize(indexDataSize);
        EnsureTransferBufferSize(vertexDataSize + indexDataSize);

        if (vertexBuffer_ != nullptr && indexBuffer_ != nullptr && transferBuffer_ != nullptr) {
            void *mapped = SDL_MapGPUTransferBuffer(device_, transferBuffer_, true);
            if (mapped != nullptr) {
                std::memcpy(mapped, vertices.data(), vertexDataSize);
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
    colorTarget.texture = targetTexture;
    colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;
    colorTarget.clear_color = {0.0F, 0.0F, 0.0F, 1.0F};

    SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTarget, 1, nullptr);
    if (renderPass == nullptr) {
        return;
    }

    if (hasCommands && vertexBuffer_ != nullptr && indexBuffer_ != nullptr) {
        SDL_GPUBufferBinding vertexBinding = {vertexBuffer_, 0};
        SDL_BindGPUVertexBuffers(renderPass, 0, &vertexBinding, 1);
        SDL_GPUBufferBinding indexBinding = {indexBuffer_, 0};
        SDL_BindGPUIndexBuffer(renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

        const float viewSize[2] = {static_cast<float>(width), static_cast<float>(height)};
        SDL_PushGPUVertexUniformData(commandBuffer, 0, viewSize, sizeof(viewSize));

        const std::vector<RenderCommand> &commands = renderQueue_.GetCommands();
        SDL_GPUGraphicsPipeline *defaultPipeline = defaultPipeline_->GetResource();
        SDL_GPUGraphicsPipeline *currentPipeline = defaultPipeline;
        SDL_BindGPUGraphicsPipeline(renderPass, currentPipeline);
        Uint32 firstIndex = 0;
        for (const RenderCommand &command: commands) {
            SDL_GPUGraphicsPipeline *commandPipeline = command.pipeline != nullptr ? command.pipeline : defaultPipeline;
            if (commandPipeline != currentPipeline) {
                SDL_BindGPUGraphicsPipeline(renderPass, commandPipeline);
                currentPipeline = commandPipeline;
            }
            SDL_GPUTexture *texture = nullptr;
            if (command.texture != nullptr && command.texture->GetResource() != nullptr) {
                texture = command.texture->GetResource();
            }
            if (texture == nullptr) {
                firstIndex += 6;
                continue;
            }
            SDL_GPUTextureSamplerBinding textureSamplerBinding = {texture, defaultSampler_->GetResource()};
            SDL_BindGPUFragmentSamplers(renderPass, 0, &textureSamplerBinding, 1);
            SDL_DrawGPUIndexedPrimitives(renderPass, 6, 1, firstIndex, 0, 0);
            firstIndex += 6;
        }
    }

    SDL_EndGPURenderPass(renderPass);
}

void glimmer::AppRenderer::FlushLightingPass(SDL_GPUCommandBuffer *commandBuffer, SDL_GPUTexture *targetTexture) {
    if (sceneTexture_ == nullptr ||
        lightMapTexture_.GetTexture() == nullptr) {
        return;
    }

    SDL_GPUColorTargetInfo colorTarget = {};
    colorTarget.texture = targetTexture;
    colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;
    colorTarget.clear_color = {0.0F, 0.0F, 0.0F, 1.0F};

    SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTarget, 1, nullptr);
    if (renderPass == nullptr) {
        return;
    }
    if (lightingPipeline_ == nullptr) {
        ResourceRef resourceRef;
        resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
        resourceRef.SetResourceType(RESOURCE_PIPELINE);
        resourceRef.SetResourceKey("lighting");
        lightingPipeline_ = resourceLocator_->FindGPUGraphicsPipeline(&resourceRef);
    }
    if (lightingPipeline_ == nullptr) {
        LogCat::e(std::source_location::current(), "lighting pipeline not found");
        return;
    }
    SDL_GPUGraphicsPipeline *pipeline = lightingPipeline_->GetResource();
    if (pipeline == nullptr) {
        LogCat::e(std::source_location::current(), "pipeline == nullptr");
        return;
    }
    if (lightingSampler_ == nullptr) {
        ResourceRef resourceRef;
        resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
        resourceRef.SetResourceType(RESOURCE_SAMPLER);
        resourceRef.SetResourceKey("lighting");
        lightingSampler_ = resourceLocator_->FindGPUGraphicsSampler(&resourceRef);
    }
    if (lightingSampler_ == nullptr) {
        LogCat::e(std::source_location::current(), "lightingSampler failed: ");
        return;
    }
    SDL_GPUSampler *sampler = lightingSampler_->GetResource();
    if (sampler == nullptr) {
        LogCat::e(std::source_location::current(), "SDL_GPUSampler == nullptr");
        return;
    }
    SDL_BindGPUGraphicsPipeline(renderPass, pipeline);
    SDL_GPUTextureSamplerBinding bindings[2] = {
        {sceneTexture_, sampler},
        {lightMapTexture_.GetTexture(), sampler}
    };
    SDL_BindGPUFragmentSamplers(renderPass, 0, bindings, 2);
    SDL_PushGPUFragmentUniformData(commandBuffer, 0, lightingParams_, sizeof(lightingParams_));
    SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);
    SDL_EndGPURenderPass(renderPass);
}

void glimmer::AppRenderer::EnsureSceneTexture(const Uint32 width, const Uint32 height) {
    if (width == 0 || height == 0) {
        return;
    }
    if (sceneTexture_ != nullptr && sceneTextureWidth_ == width && sceneTextureHeight_ == height) {
        return;
    }
    if (sceneTexture_ != nullptr) {
        SDL_ReleaseGPUTexture(device_, sceneTexture_);
        sceneTexture_ = nullptr;
    }
    SDL_GPUTextureCreateInfo info = {};
    info.type = SDL_GPU_TEXTURETYPE_2D;
    info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    info.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
    info.width = width;
    info.height = height;
    info.layer_count_or_depth = 1;
    info.num_levels = 1;
    info.sample_count = SDL_GPU_SAMPLECOUNT_1;
    info.props = 0;
    sceneTexture_ = SDL_CreateGPUTexture(device_, &info);
    if (sceneTexture_ == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_CreateGPUTexture failed: ", SDL_GetError());
        return;
    }
    sceneTextureWidth_ = width;
    sceneTextureHeight_ = height;
}


void glimmer::AppRenderer::UpdateLightMap(const LightBuffer *lightBuffer, const CameraComponent *camera,
                                          const Transform2DComponent *cameraTransform,
                                          const WorldContext *worldContext,
                                          const Uint32 width, const Uint32 height) {
    if (lightBuffer == nullptr || camera == nullptr || cameraTransform == nullptr) {
        return;
    }
    const float zoom = camera->GetZoom();
    const ScreenVector2D cameraSize(static_cast<float>(width), static_cast<float>(height));
    const WorldVector2D cameraPosition = cameraTransform->GetPosition();
    const SDL_FRect viewportRect = CoordinateTransformer::GetViewportRect(cameraPosition, cameraSize, zoom);
    const TileVector2D tileMin = CoordinateTransformer::WorldToTile(
        WorldVector2D(viewportRect.x, viewportRect.y));
    const TileVector2D tileMax = CoordinateTransformer::WorldToTile(
        WorldVector2D(viewportRect.x + viewportRect.w, viewportRect.y + viewportRect.h));
    const int originX = tileMin.x - 1;
    const int originY = tileMin.y - 1;
    const auto sizeX = static_cast<Uint32>(tileMax.x - tileMin.x + 3);
    const auto sizeY = static_cast<Uint32>(tileMax.y - tileMin.y + 3);
    const Config *config = appContext_->GetConfig();
    const bool fullBright = config == nullptr || !config->light.enable;
    const AmbientLight ambient = ComputeAmbientLight(worldContext != nullptr ? worldContext->GetTimeOfDay() : 12.0F);
    lightMapTexture_.Update(device_, lightBuffer, fullBright ? nullptr : &ambient,
                            originX, originY, sizeX, sizeY, fullBright);

    const LightingConfig &lighting = config != nullptr ? config->lighting : LightingConfig{};
    lightingParams_[0] = static_cast<float>(originX);
    lightingParams_[1] = static_cast<float>(originY);
    lightingParams_[2] = static_cast<float>(sizeX);
    lightingParams_[3] = static_cast<float>(sizeY);
    lightingParams_[4] = cameraPosition.x;
    lightingParams_[5] = cameraPosition.y;
    lightingParams_[6] = cameraSize.x;
    lightingParams_[7] = cameraSize.y;
    lightingParams_[8] = zoom;
    lightingParams_[9] = static_cast<float>(TILE_SIZE);
    lightingParams_[10] = lighting.fullBrightAlpha;
    lightingParams_[11] = lighting.minVisibility;
    lightingParams_[12] = lighting.tintStrength;
    lightingParams_[13] = 0.0F;
    lightingParams_[14] = 0.0F;
    lightingParams_[15] = 0.0F;
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
