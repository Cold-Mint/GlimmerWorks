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
#include "SkySystem.h"

#include "core/config/Constants.h"
#include "core/context/AppContext.h"
#include "core/context/WindowContext.h"
#include "core/gpu/RenderQueue.h"
#include "core/mod/Resource.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/ResourceRef.h"
#include "core/mod/resourcePack/GPUPipelineResourceResult.h"
#include "core/mod/resourcePack/GPUSamplerResourceResult.h"
#include "core/world/AmbientLight.h"
#include "core/world/Dimension.h"
#include "core/world/WorldContext.h"
#include "core/world/weather/WeatherManager.h"

glimmer::SkySystem::SkySystem(WorldContext *worldContext) : GameSystem(worldContext) {
    Init();
}

void glimmer::SkySystem::Render(RenderQueue *queue) {
    WorldContext *worldContext = GetWorldContext();
    if (worldContext == nullptr || queue == nullptr) {
        return;
    }
    AppContext *appContext = worldContext->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    ResourceLocator *resourceLocator = appContext->GetResourceLocator();
    if (resourceLocator == nullptr) {
        return;
    }
    WindowContext *windowContext = appContext->GetWindowContext();
    if (windowContext == nullptr) {
        return;
    }
    const SDL_FRect fullRect = {
        0.0F, 0.0F,
        static_cast<float>(windowContext->GetWindowWidth()),
        static_cast<float>(windowContext->GetWindowHeight())
    };

    // 1. Base sky gradient.
    // 1. 天空底色渐变。
    ResourceRef skyPipelineRef;
    skyPipelineRef.SetPackageId(RESOURCE_REF_CORE);
    skyPipelineRef.SetResourceType(RESOURCE_PIPELINE);
    skyPipelineRef.SetResourceKey("sky");
    const std::shared_ptr<GPUPipelineResourceResult> skyPipeline = resourceLocator->FindGPUGraphicsPipeline(
        &skyPipelineRef, false);
    if (skyPipeline != nullptr && skyPipeline->GetResource() != nullptr) {
        queue->DrawFullScreenQuad(RenderLayer::Background, 0.0F, &fullRect,
                                  skyPipeline->GetResource(), nullptr, skyPipeline->GetUniformBlock());
    } else {
        // Fallback: flat horizon color.
        // 回退：平坦的地平线颜色。
        const SkyColors sky = ComputeSkyColors(worldContext, worldContext->GetTimeOfDay());
        queue->FillRect(RenderLayer::Background, 0.0F, &fullRect, sky.horizon.ToSDLColor());
    }

    // 2. Sky elements from the active weathers.
    // 2. 来自生效天气的天空元素。
    Dimension *dimension = worldContext->GetCurrentDimension();
    if (dimension == nullptr || dimension->GetWeatherManager() == nullptr) {
        return;
    }
    const std::vector<const SkyElementResource *> elements = dimension->GetWeatherManager()->GetActiveElements();
    for (const SkyElementResource *element: elements) {
        if (element == nullptr || !element->pipeline.IsValid()) {
            continue;
        }
        const std::shared_ptr<GPUPipelineResourceResult> pipeline = resourceLocator->FindGPUGraphicsPipeline(
            &element->pipeline, false);
        if (pipeline == nullptr || pipeline->GetResource() == nullptr) {
            continue;
        }
        SDL_GPUSampler *sampler = nullptr;
        if (element->sampler.IsValid()) {
            const std::shared_ptr<GPUSamplerResourceResult> samplerResult = resourceLocator->FindGPUGraphicsSampler(
                &element->sampler, false);
            if (samplerResult != nullptr) {
                sampler = samplerResult->GetResource();
            }
        }
        queue->DrawFullScreenQuad(RenderLayer::Background, 0.0F, &fullRect,
                                  pipeline->GetResource(), sampler, pipeline->GetUniformBlock());
    }
}

glimmer::GameSystemType glimmer::SkySystem::GetGameSystemType() const {
    return GameSystemType::SkySystem;
}

uint8_t glimmer::SkySystem::GetExecutionOrder() {
    return EXECUTION_ORDER_PARALLAX_BACKGROUND;
}
