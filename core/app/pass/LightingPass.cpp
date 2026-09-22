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
#include "LightingPass.h"

#include <algorithm>
#include <cstring>
#include <vector>

#include "PassUtils.h"
#include "core/gpu/RenderFrameContext.h"
#include "core/gpu/SpriteVertex.h"
#include "core/gpu/UniformInjectContext.h"
#include "core/log/LogCat.h"
#include "core/math/CoordinateTransformer.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/ResourceRef.h"
#include "core/mod/resourcePack/GPUPipelineResourceResult.h"
#include "core/mod/resourcePack/GPUSamplerResourceResult.h"
#include "core/utils/ColorUtils.h"
#include "core/world/Dimension.h"
#include "core/world/WorldContext.h"


glimmer::LightingPass::LightingPass(ResourceLocator *resourceLocator, SDL_GPUDevice *device,
                                    std::shared_ptr<GPUPipelineResourceResult> lightingPipeline,
                                    std::shared_ptr<GPUSamplerResourceResult> lightingSampler)
    : resourceLocator_(resourceLocator),
      device_(device),
      lightingPipeline_(std::move(lightingPipeline)),
      lightingSampler_(std::move(lightingSampler)) {
#if  !defined(NDEBUG)
    if (resourceLocator_ != nullptr) {
        ResourceRef defaultPipelineResourceRef;
        defaultPipelineResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
        defaultPipelineResourceRef.SetResourceType(RESOURCE_PIPELINE);
        defaultPipelineResourceRef.SetResourceKey("default");
        debugPipeline_ = resourceLocator_->FindGPUGraphicsPipeline(&defaultPipelineResourceRef);

        ResourceRef defaultSamplerResourceRef;
        defaultSamplerResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
        defaultSamplerResourceRef.SetResourceType(RESOURCE_SAMPLER);
        defaultSamplerResourceRef.SetResourceKey("default");
        debugSampler_ = resourceLocator_->FindGPUGraphicsSampler(&defaultSamplerResourceRef);
    }
#endif
}

glimmer::LightingPass::~LightingPass() {
#if  !defined(NDEBUG)
    if (debugTransferBuffer_ != nullptr) {
        SDL_ReleaseGPUTransferBuffer(device_, debugTransferBuffer_);
        debugTransferBuffer_ = nullptr;
    }
    if (debugIndexBuffer_ != nullptr) {
        SDL_ReleaseGPUBuffer(device_, debugIndexBuffer_);
        debugIndexBuffer_ = nullptr;
    }
    if (debugVertexBuffer_ != nullptr) {
        SDL_ReleaseGPUBuffer(device_, debugVertexBuffer_);
        debugVertexBuffer_ = nullptr;
    }
#endif
}

void glimmer::LightingPass::Prepare(RenderFrameContext &ctx) {
#if  !defined(NDEBUG)
    if (!enableLighting_) {
        return;
    }
#endif
    if (ctx.injectContext != nullptr) {
        UpdateLightMap(ctx.injectContext);
    }
}

void glimmer::LightingPass::Record(RenderFrameContext &ctx) {
#if  !defined(NDEBUG)
    if (!enableLighting_) {
        BlitScene(ctx);
        return;
    }
#endif
    if (ctx.injectContext == nullptr) {
        return;
    }
    lightMapTexture_.Upload(ctx.commandBuffer);
    FlushLightingPass(ctx);
#if  !defined(NDEBUG)
    if (displayLightMap_) {
        DrawLightMapDebug(ctx);
    }
#endif
}

void glimmer::LightingPass::FlushLightingPass(RenderFrameContext &ctx) {
    SDL_GPUCommandBuffer *commandBuffer = ctx.commandBuffer;
    SDL_GPUTexture *targetTexture = ctx.swapChainTexture;
    const UniformInjectContext *injectContext = ctx.injectContext;
    if (ctx.sceneTexture == nullptr) {
        LogCat::w(std::source_location::current(), "app_renderer_scene_texture_is_null",
                  "sceneTexture_ is null, lighting pass skipped");
        return;
    }
    if (lightMapTexture_.GetTexture() == nullptr) {
        LogCat::w(std::source_location::current(), "app_renderer_light_map_texture_is_null",
                  "lightMapTexture_ is null, lighting pass skipped");
        return;
    }

    SDL_GPUColorTargetInfo colorTarget = {};
    colorTarget.texture = targetTexture;
    colorTarget.load_op = SDL_GPU_LOADOP_LOAD;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;

    SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTarget, 1, nullptr);
    if (renderPass == nullptr) {
        LogCat::e(std::source_location::current(), "render_pass_is_null", "renderPass == nullptr");
        return;
    }
    if (lightingPipeline_ == nullptr) {
        LogCat::e(std::source_location::current(), "lighting_pipeline_not_found", "lighting pipeline not found");
        return;
    }
    SDL_GPUGraphicsPipeline *pipeline = lightingPipeline_->GetResource();
    if (pipeline == nullptr) {
        LogCat::e(std::source_location::current(), "pipeline_is_null", "pipeline == nullptr");
        return;
    }
    if (lightingSampler_ == nullptr) {
        LogCat::e(std::source_location::current(), "lighting_sampler_failed", "lightingSampler failed: ");
        return;
    }
    SDL_GPUSampler *sampler = lightingSampler_->GetResource();
    if (sampler == nullptr) {
        LogCat::e(std::source_location::current(), "sdl_gpu_sampler_is_null", "SDL_GPUSampler == nullptr");
        return;
    }
    SDL_BindGPUGraphicsPipeline(renderPass, pipeline);
    SDL_GPUTextureSamplerBinding bindings[2] = {
        {ctx.sceneTexture, sampler},
        {lightMapTexture_.GetTexture(), sampler}
    };
    SDL_BindGPUFragmentSamplers(renderPass, 0, bindings, 2);
    FillAndPushUniformBlock(commandBuffer, lightingPipeline_->GetUniformBlocks(), *injectContext,
                            lightingStagingBuffer_);
    SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);
    SDL_EndGPURenderPass(renderPass);
}

void glimmer::LightingPass::UpdateLightMap(UniformInjectContext *injectContext) {
    const float zoom = injectContext->camera->GetZoom();
    const ScreenVector2D cameraSize(injectContext->width, injectContext->height);
    const WorldVector2D cameraPosition = injectContext->cameraTransform->GetPosition();
    const SDL_FRect viewportRect = CoordinateTransformer::GetViewportRect(cameraPosition, cameraSize, zoom);
    const TileVector2D tileMin = CoordinateTransformer::WorldToTile(
        WorldVector2D(viewportRect.x, viewportRect.y));
    const TileVector2D tileMax = CoordinateTransformer::WorldToTile(
        WorldVector2D(viewportRect.x + viewportRect.w, viewportRect.y + viewportRect.h));
    const int originX = tileMin.x - 1;
    const int originY = tileMin.y - 1;
    const auto sizeX = static_cast<Uint32>(tileMax.x - tileMin.x + 3);
    const auto sizeY = static_cast<Uint32>(tileMax.y - tileMin.y + 3);
    LogCat::i("app_renderer_light_map_camera",
              "UpdateLightMap camera: zoom={}, cameraSize=({},{}), cameraPosition=({},{})",
              zoom, cameraSize.x, cameraSize.y, cameraPosition.x, cameraPosition.y);
    LogCat::i("app_renderer_light_map_tile_range",
              "UpdateLightMap tile range: tileMin=({},{}), tileMax=({},{})",
              tileMin.x, tileMin.y, tileMax.x, tileMax.y);
    LogCat::i("app_renderer_light_map_info",
              "UpdateLightMap: viewport=({},{},{},{}), origin=({},{}), size=({}x{})",
              viewportRect.x, viewportRect.y, viewportRect.w, viewportRect.h,
              originX, originY, sizeX, sizeY);

    WorldContext *worldContext = injectContext->worldContext;
    const DimensionResource *dimensionResource = nullptr;
    Dimension *dimension = nullptr;
    if (worldContext == nullptr) {
        LogCat::w(std::source_location::current(), "world_context_is_null", "worldContext == nullptr");
        return;
    }
    dimension = worldContext->GetDimension();
    if (dimension == nullptr) {
        LogCat::w(std::source_location::current(), "dimension_is_null", "dimension is nullptr");
        return;
    }
    dimensionResource = dimension->GetDimensionResource();
    if (dimensionResource == nullptr) {
        LogCat::w(std::source_location::current(), "dimension_resource_is_null", "dimensionResource == nullptr");
        return;
    }
    const float timeOfDay = dimension->GetNormalizedTime();
    const std::vector<LightKeyframe> &backLightKeyFrames = dimensionResource->backLightKeyframes;
    const std::vector<LightKeyframe> &skyKeyframes = dimensionResource->skyLightKeyframes;
    const Color screenLight = ColorUtils::ComputeAmbientLight(resourceLocator_, timeOfDay, backLightKeyFrames);
    const Color skyLight = ColorUtils::ComputeAmbientLight(resourceLocator_, timeOfDay, skyKeyframes);
    LogCat::i("app_renderer_ambient_light",
              "Ambient light: screen rgba=({},{},{},{}), sky rgba=({},{},{},{}), timeOfDay={}, backLightKeyFrames={}, skyKeyframes={}",
              static_cast<int>(screenLight.r), static_cast<int>(screenLight.g),
              static_cast<int>(screenLight.b), static_cast<int>(screenLight.a),
              static_cast<int>(skyLight.r), static_cast<int>(skyLight.g),
              static_cast<int>(skyLight.b), static_cast<int>(skyLight.a), timeOfDay,
              backLightKeyFrames.size(), skyKeyframes.size());

    //Fold the resolved colors into the light buffer as screen light (background
    //layer) and sky light (from above), which are configured independently.
    //将解析出的颜色并入光照缓冲，作为屏幕光（背景层）与天光（上方），二者独立配置。
    if (injectContext->lightBuffer != nullptr) {
        injectContext->lightBuffer->SetLightColor(screenLight, skyLight);
        //Coalesce every light mutation of this frame into a single recompute
        //right before the light map is sampled.
        //在光图被采样之前，将本帧所有光照修改合并为一次重算。
        injectContext->lightBuffer->Flush();
    }
    lightMapTexture_.Update(device_, injectContext->lightBuffer,
                            originX, originY, sizeX, sizeY);
    injectContext->lightMapOriginX = originX;
    injectContext->lightMapOriginY = originY;
    injectContext->lightMapSizeX = sizeX;
    injectContext->lightMapSizeY = sizeY;
}

#if  !defined(NDEBUG)
void glimmer::LightingPass::SetDisplayLightMap(const bool display) {
    displayLightMap_ = display;
}

void glimmer::LightingPass::SetEnableLighting(const bool enable) {
    enableLighting_ = enable;
}

void glimmer::LightingPass::BlitScene(RenderFrameContext &ctx) {
    //When lighting is disabled, draw the unlit scene texture to the
    //swapchain using a fullscreen quad, bypassing the lighting composite.
    //禁用光照时，用全屏四边形把无光照场景纹理绘制到交换链，绕过光照合成。
    SDL_GPUTexture *sceneTexture = ctx.sceneTexture;
    if (ctx.commandBuffer == nullptr || ctx.swapChainTexture == nullptr || sceneTexture == nullptr) {
        return;
    }
    if (debugPipeline_ == nullptr || debugSampler_ == nullptr || debugPipeline_->GetResource() == nullptr ||
        debugSampler_->GetResource() == nullptr) {
        return;
    }
    EnsureDebugBuffers();
    if (debugVertexBuffer_ == nullptr || debugIndexBuffer_ == nullptr || debugTransferBuffer_ == nullptr) {
        return;
    }

    const float logicalW = static_cast<float>(ctx.logicalWidth);
    const float logicalH = static_cast<float>(ctx.logicalHeight);
    //Build a fullscreen quad in logical pixel coordinates with a white
    //vertex color, so the sprite shader outputs the sampled texture as-is.
    //构建逻辑像素坐标下的全屏四边形，顶点色为白色，使精灵着色器原样输出采样纹理。
    const SpriteVertex vertices[4] = {
        {0.0F, 0.0F, 0.0F, 0.0F, 255, 255, 255, 255},
        {logicalW, 0.0F, 1.0F, 0.0F, 255, 255, 255, 255},
        {0.0F, logicalH, 0.0F, 1.0F, 255, 255, 255, 255},
        {logicalW, logicalH, 1.0F, 1.0F, 255, 255, 255, 255}
    };
    const Uint32 indices[6] = {0, 1, 2, 1, 3, 2};

    //Upload the quad vertices and indices into the shared debug transfer
    //buffer, then copy them into the vertex/index buffers in a copy pass.
    //将四边形顶点与索引上传到共享的调试传输缓冲，再通过复制通道写入顶点/索引缓冲。
    void *mapped = SDL_MapGPUTransferBuffer(device_, debugTransferBuffer_, true);
    if (mapped != nullptr) {
        std::memcpy(mapped, vertices, sizeof(vertices));
        std::memcpy(static_cast<Uint8 *>(mapped) + sizeof(vertices), indices, sizeof(indices));
        SDL_UnmapGPUTransferBuffer(device_, debugTransferBuffer_);
    }

    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(ctx.commandBuffer);
    if (copyPass == nullptr) {
        return;
    }
    SDL_GPUTransferBufferLocation vertexSource = {debugTransferBuffer_, 0};
    SDL_GPUBufferRegion vertexDestination = {debugVertexBuffer_, 0, sizeof(vertices)};
    SDL_UploadToGPUBuffer(copyPass, &vertexSource, &vertexDestination, true);
    SDL_GPUTransferBufferLocation indexSource = {debugTransferBuffer_, sizeof(vertices)};
    SDL_GPUBufferRegion indexDestination = {debugIndexBuffer_, 0, sizeof(indices)};
    SDL_UploadToGPUBuffer(copyPass, &indexSource, &indexDestination, true);
    SDL_EndGPUCopyPass(copyPass);

    //Use LOADOP_LOAD so the fullscreen quad is blended over the already
    //cleared swapchain; transparent scene regions keep the black clear color.
    //使用 LOADOP_LOAD 使全屏四边形叠加在已清屏的交换链上；场景透明区域保留黑色清屏色。
    SDL_GPUColorTargetInfo colorTarget = {};
    colorTarget.texture = ctx.swapChainTexture;
    colorTarget.load_op = SDL_GPU_LOADOP_LOAD;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;

    SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(ctx.commandBuffer, &colorTarget, 1, nullptr);
    if (renderPass == nullptr) {
        return;
    }
    SDL_BindGPUGraphicsPipeline(renderPass, debugPipeline_->GetResource());

    //Fill the debug pipeline's uniform blocks from the shared injection context
    //and push them, so the quad uses the same camera/viewport transform as the scene.
    //用共享注入上下文填充调试管线的 uniform 块并推送，使四边形使用与场景相同的相机/视口变换。
    if (ctx.injectContext != nullptr) {
        FillAndPushUniformBlock(ctx.commandBuffer, debugPipeline_->GetUniformBlocks(), *ctx.injectContext,
                                lightingStagingBuffer_);
    }

    SDL_GPUBufferBinding vertexBinding = {debugVertexBuffer_, 0};
    SDL_BindGPUVertexBuffers(renderPass, 0, &vertexBinding, 1);
    SDL_GPUBufferBinding indexBinding = {debugIndexBuffer_, 0};
    SDL_BindGPUIndexBuffer(renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

    //Sample the unlit scene texture with the debug sampler and draw the quad.
    //用调试采样器采样无光照场景纹理并绘制四边形。
    SDL_GPUTextureSamplerBinding textureSamplerBinding = {sceneTexture, debugSampler_->GetResource()};
    SDL_BindGPUFragmentSamplers(renderPass, 0, &textureSamplerBinding, 1);

    SDL_DrawGPUIndexedPrimitives(renderPass, 6, 1, 0, 0, 0);
    SDL_EndGPURenderPass(renderPass);
}

void glimmer::LightingPass::EnsureDebugBuffers() {
    if (device_ == nullptr) {
        return;
    }
    //Lazily create the fixed-size vertex buffer holding the fullscreen quad.
    //懒创建保存全屏四边形的固定大小顶点缓冲。
    if (debugVertexBuffer_ == nullptr) {
        SDL_GPUBufferCreateInfo info = {};
        info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
        info.size = sizeof(SpriteVertex) * 4;
        debugVertexBuffer_ = SDL_CreateGPUBuffer(device_, &info);
    }
    //Lazily create the fixed-size index buffer for the two triangles.
    //懒创建两个三角形所需的固定大小索引缓冲。
    if (debugIndexBuffer_ == nullptr) {
        SDL_GPUBufferCreateInfo info = {};
        info.usage = SDL_GPU_BUFFERUSAGE_INDEX;
        info.size = sizeof(Uint32) * 6;
        debugIndexBuffer_ = SDL_CreateGPUBuffer(device_, &info);
    }
    const Uint32 totalSize = sizeof(SpriteVertex) * 4 + sizeof(Uint32) * 6;
    //(Re)allocate the upload staging buffer only when it is missing or too
    //small; otherwise reuse it across frames to avoid per-frame allocation.
    //仅当上传暂存缓冲缺失或过小时才（重新）分配，否则跨帧复用，避免每帧分配。
    if (debugTransferBuffer_ == nullptr || debugTransferBufferSize_ < totalSize) {
        if (debugTransferBuffer_ != nullptr) {
            SDL_ReleaseGPUTransferBuffer(device_, debugTransferBuffer_);
            debugTransferBuffer_ = nullptr;
            debugTransferBufferSize_ = 0;
        }
        SDL_GPUTransferBufferCreateInfo info = {};
        info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        info.size = totalSize;
        debugTransferBuffer_ = SDL_CreateGPUTransferBuffer(device_, &info);
        debugTransferBufferSize_ = debugTransferBuffer_ != nullptr ? totalSize : 0;
    }
}

void glimmer::LightingPass::DrawLightMapDebug(RenderFrameContext &ctx) {
    SDL_GPUTexture *lightMapTexture = lightMapTexture_.GetTexture();
    if (ctx.commandBuffer == nullptr || ctx.swapChainTexture == nullptr || lightMapTexture == nullptr) {
        return;
    }
    if (debugPipeline_ == nullptr || debugSampler_ == nullptr || debugPipeline_->GetResource() == nullptr ||
        debugSampler_->GetResource() == nullptr) {
        return;
    }
    EnsureDebugBuffers();
    if (debugVertexBuffer_ == nullptr || debugIndexBuffer_ == nullptr || debugTransferBuffer_ == nullptr) {
        return;
    }

    const float logicalW = static_cast<float>(ctx.logicalWidth);
    const float logicalH = static_cast<float>(ctx.logicalHeight);
    constexpr float margin = 16.0F;
    constexpr float maxSize = 256.0F;
    const float texW = static_cast<float>(lightMapTexture_.GetWidth());
    const float texH = static_cast<float>(lightMapTexture_.GetHeight());
    float w = maxSize;
    float h = maxSize;
    if (texW > 0.0F && texH > 0.0F) {
        const float scale = maxSize / std::max(texW, texH);
        w = texW * scale;
        h = texH * scale;
    }
    const float x = logicalW - w - margin;
    const float y = logicalH - h - margin;

    const SpriteVertex vertices[4] = {
        {x, y, 0.0F, 0.0F, 255, 255, 255, 255},
        {x + w, y, 1.0F, 0.0F, 255, 255, 255, 255},
        {x, y + h, 0.0F, 1.0F, 255, 255, 255, 255},
        {x + w, y + h, 1.0F, 1.0F, 255, 255, 255, 255}
    };
    const Uint32 indices[6] = {0, 1, 2, 1, 3, 2};

    void *mapped = SDL_MapGPUTransferBuffer(device_, debugTransferBuffer_, true);
    if (mapped != nullptr) {
        std::memcpy(mapped, vertices, sizeof(vertices));
        std::memcpy(static_cast<Uint8 *>(mapped) + sizeof(vertices), indices, sizeof(indices));
        SDL_UnmapGPUTransferBuffer(device_, debugTransferBuffer_);
    }

    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(ctx.commandBuffer);
    if (copyPass == nullptr) {
        return;
    }
    SDL_GPUTransferBufferLocation vertexSource = {debugTransferBuffer_, 0};
    SDL_GPUBufferRegion vertexDestination = {debugVertexBuffer_, 0, sizeof(vertices)};
    SDL_UploadToGPUBuffer(copyPass, &vertexSource, &vertexDestination, true);
    SDL_GPUTransferBufferLocation indexSource = {debugTransferBuffer_, sizeof(vertices)};
    SDL_GPUBufferRegion indexDestination = {debugIndexBuffer_, 0, sizeof(indices)};
    SDL_UploadToGPUBuffer(copyPass, &indexSource, &indexDestination, true);
    SDL_EndGPUCopyPass(copyPass);

    SDL_GPUColorTargetInfo colorTarget = {};
    colorTarget.texture = ctx.swapChainTexture;
    colorTarget.load_op = SDL_GPU_LOADOP_LOAD;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;

    SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(ctx.commandBuffer, &colorTarget, 1, nullptr);
    if (renderPass == nullptr) {
        return;
    }
    SDL_BindGPUGraphicsPipeline(renderPass, debugPipeline_->GetResource());

    //Fill the debug pipeline's uniform blocks from the shared injection context
    //and push them, so the quad uses the same camera/viewport transform as the scene.
    //用共享注入上下文填充调试管线的 uniform 块并推送，使四边形使用与场景相同的相机/视口变换。
    if (ctx.injectContext != nullptr) {
        FillAndPushUniformBlock(ctx.commandBuffer, debugPipeline_->GetUniformBlocks(), *ctx.injectContext,
                                lightingStagingBuffer_);
    }

    SDL_GPUBufferBinding vertexBinding = {debugVertexBuffer_, 0};
    SDL_BindGPUVertexBuffers(renderPass, 0, &vertexBinding, 1);
    SDL_GPUBufferBinding indexBinding = {debugIndexBuffer_, 0};
    SDL_BindGPUIndexBuffer(renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

    SDL_GPUTextureSamplerBinding textureSamplerBinding = {lightMapTexture, debugSampler_->GetResource()};
    SDL_BindGPUFragmentSamplers(renderPass, 0, &textureSamplerBinding, 1);

    SDL_DrawGPUIndexedPrimitives(renderPass, 6, 1, 0, 0, 0);
    SDL_EndGPURenderPass(renderPass);
}
#endif
