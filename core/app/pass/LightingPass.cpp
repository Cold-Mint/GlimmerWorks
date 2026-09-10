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

#include <vector>

#include "PassUtils.h"
#include "core/gpu/RenderFrameContext.h"
#include "core/gpu/UniformInjectContext.h"
#include "core/log/LogCat.h"
#include "core/math/CoordinateTransformer.h"
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
}

void glimmer::LightingPass::Prepare(RenderFrameContext &ctx) {
    if (ctx.injectContext != nullptr) {
        UpdateLightMap(ctx.injectContext);
    }
}

void glimmer::LightingPass::Record(RenderFrameContext &ctx) {
    if (ctx.injectContext == nullptr) {
        return;
    }
    lightMapTexture_.Upload(ctx.commandBuffer);
    FlushLightingPass(ctx);
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
    FillAndPushUniformBlock(commandBuffer, lightingPipeline_, *injectContext, lightingStagingBuffer_);
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
    const float timeOfDay = dimensionResource->initialTime;
    const std::vector<LightKeyframe> &keyframes = dimensionResource->ambientLightKeyframes;
    ambientLight_ = ColorUtils::ComputeAmbientLight(resourceLocator_, timeOfDay, keyframes);
    LogCat::i("app_renderer_ambient_light", "Ambient light: rgba=({},{},{},{}), timeOfDay={}, keyframes={}",
              static_cast<int>(ambientLight_.r), static_cast<int>(ambientLight_.g),
              static_cast<int>(ambientLight_.b), static_cast<int>(ambientLight_.a), timeOfDay,
              keyframes.size());


    lightMapTexture_.Update(device_, injectContext->lightBuffer, &ambientLight_,
                            originX, originY, sizeX, sizeY);
    injectContext->lightMapOriginX = originX;
    injectContext->lightMapOriginY = originY;
    injectContext->lightMapSizeX = sizeX;
    injectContext->lightMapSizeY = sizeY;
}
