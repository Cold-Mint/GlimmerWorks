//
// Created by Cold-Mint on 2026/2/26.
//

#include "AreaMarkerSystem.h"

#include "core/ecs/component/AreaMarkerComponent.h"
#include "../../world/WorldContext.h"
#include "fmt/color.h"

glimmer::AreaMarkerSystem::AreaMarkerSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent<AreaMarkerComponent>();
}


void glimmer::AreaMarkerSystem::Update(const float delta) {
    auto entityList = worldContext_->GetEntityIDWithComponents<AreaMarkerComponent>();
    for (const auto entity: entityList) {
        auto *areaMarker = worldContext_->GetComponent<AreaMarkerComponent>(entity);
        if (areaMarker == nullptr) {
            continue;
        }
        if (!areaMarker->CanDraw()) {
            continue;
        }
        areaMarker->SetRemainingTime(areaMarker->GetRemainingTime() - delta);
        if (areaMarker->IsExpired()) {
            areaMarker->Reset();
        }
    }
}

void glimmer::AreaMarkerSystem::Render(SDL_Renderer *renderer) {
    const AppContext *appContext = worldContext_->GetAppContext();
    const auto *cameraComponent = worldContext_->GetCameraComponent();
    const auto *cameraPos = worldContext_->GetCameraTransform2D();
    if (cameraComponent == nullptr) {
        return;
    }
    if (cameraPos == nullptr) {
        return;
    }
    const auto entityList = worldContext_->GetEntityIDWithComponents<AreaMarkerComponent>();
    for (auto entity: entityList) {
        const auto *areaMarker = worldContext_->GetComponent<AreaMarkerComponent>(entity);
        if (areaMarker == nullptr) {
            continue;
        }
        if (!areaMarker->CanDraw()) {
            continue;
        }
        const auto &areaMarkerFullColor = worldContext_->GetAppContext()->GetPreloadColors()->
                areaMarkerColor;
        const auto &areaMarkerBorderColor = worldContext_->GetAppContext()->GetPreloadColors()->
                areaMarkerBorderColor;
        WorldVector2D startPoint = TileLayerComponent::TileToWorld(areaMarker->GetStartPoint());
        WorldVector2D endPoint = TileLayerComponent::TileToWorld(areaMarker->GetEndPoint());
        int tileWidth = std::abs(areaMarker->GetEndPoint().x - areaMarker->GetStartPoint().x) + 1;
        int tileHeight = std::abs(areaMarker->GetEndPoint().y - areaMarker->GetStartPoint().y) + 1;
        int tileArea = tileWidth * tileHeight;
        const float minWorldX = std::min(startPoint.x, endPoint.x);
        const float maxWorldX = std::max(startPoint.x, endPoint.x);
        const float minWorldY = std::min(startPoint.y, endPoint.y);
        const float maxWorldY = std::max(startPoint.y, endPoint.y);
        WorldVector2D rectWorldMin;
        rectWorldMin.x = minWorldX - HALF_TILE_SIZE;
        rectWorldMin.y = minWorldY - HALF_TILE_SIZE;
        WorldVector2D rectWorldMax;
        rectWorldMax.x = maxWorldX + HALF_TILE_SIZE;
        rectWorldMax.y = maxWorldY + HALF_TILE_SIZE;
        if (cameraComponent->IsPointInViewport(cameraPos->GetPosition(), rectWorldMin) ||
            cameraComponent->IsPointInViewport(cameraPos->GetPosition(), rectWorldMax)) {
            CameraVector2D camMin = cameraComponent->GetViewPortPosition(cameraPos->GetPosition(), rectWorldMin);
            CameraVector2D camMax = cameraComponent->GetViewPortPosition(cameraPos->GetPosition(), rectWorldMax);
            SDL_FRect rect;
            rect.x = std::min(camMin.x, camMax.x);
            rect.y = std::min(camMin.y, camMax.y);
            rect.w = std::abs(camMax.x - camMin.x);
            rect.h = std::abs(camMax.y - camMin.y);
            SDL_SetRenderDrawColor(renderer, areaMarkerFullColor.r,
                                   areaMarkerFullColor.g, areaMarkerFullColor.b,
                                   areaMarkerFullColor.a);
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, areaMarkerBorderColor.r,
                                   areaMarkerBorderColor.g, areaMarkerBorderColor.b,
                                   areaMarkerBorderColor.a);
            SDL_RenderRect(renderer, &rect);

            std::string text = fmt::format(fmt::runtime(appContext->GetLangsResources()->areaMarkerTip),
                                           tileWidth, tileHeight, tileArea);
            SDL_Surface *ttfSurface = TTF_RenderText_Blended_Wrapped(appContext->GetFont(),
                                                                     text.c_str(), text.length(),
                                                                     appContext->GetPreloadColors()
                                                                     ->textColor.ToSDLColor(), 0);
            if (ttfSurface == nullptr) {
                continue;
            }
            const auto ttfSurfaceWidth = static_cast<float>(ttfSurface->w);
            const auto ttfSurfaceHeight = static_cast<float>(ttfSurface->h);
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, ttfSurface);
            if (texture == nullptr) {
                continue;
            }
            const float textX = rect.x + (rect.w - ttfSurfaceWidth) / 2.0F;
            const float textY = rect.y + (rect.h - ttfSurfaceHeight) / 2.0F;
            SDL_FRect rectCenter = {
                textX, textY,
                ttfSurfaceWidth,
                ttfSurfaceHeight
            };
            SDL_RenderTexture(renderer, texture, nullptr, &rectCenter);
            SDL_DestroyTexture(texture);
            SDL_DestroySurface(ttfSurface);
        }
    }
    AppContext::RestoreColorRenderer(renderer);
}


std::string glimmer::AreaMarkerSystem::GetName() {
    return "glimmer.AreaMarkerSystem";
}

uint8_t glimmer::AreaMarkerSystem::GetRenderOrder() {
    return RENDER_ORDER_AREA_MARKER;
}
