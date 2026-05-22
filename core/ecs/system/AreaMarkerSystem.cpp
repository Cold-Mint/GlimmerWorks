//
// Created by Cold-Mint on 2026/2/26.
//

#include "AreaMarkerSystem.h"

#include "core/ecs/component/AreaMarkerComponent.h"
#include "../../world/WorldContext.h"
#include "fmt/color.h"

glimmer::AreaMarkerSystem::AreaMarkerSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent<AreaMarkerComponent>();
    appContext_ = worldContext->GetAppContext();
    areaMarkerComponent_ = worldContext->GetAreaMarkerComponent();
    preloadColors_ = appContext_->GetPreloadColors();
    cameraComponent_ = worldContext->GetCameraComponent();
    cameraTransform2DComponent_ = worldContext->GetCameraTransform2D();
}


void glimmer::AreaMarkerSystem::Update(const float delta) {
    if (areaMarkerComponent_ == nullptr) {
        return;;
    }
    if (!areaMarkerComponent_->CanDraw()) {
        return;
    }
    areaMarkerComponent_->SetRemainingTime(areaMarkerComponent_->GetRemainingTime() - delta);
    if (areaMarkerComponent_->IsExpired()) {
        areaMarkerComponent_->Reset();
    }
}

void glimmer::AreaMarkerSystem::Render(SDL_Renderer *renderer) {
    if (appContext_ == nullptr) {
        return;
    }
    if (cameraComponent_ == nullptr) {
        return;
    }
    if (cameraTransform2DComponent_ == nullptr) {
        return;
    }
    if (areaMarkerComponent_ == nullptr) {
        return;;
    }
    if (!areaMarkerComponent_->CanDraw()) {
        return;
    }
    WorldVector2D cameraPosition = cameraTransform2DComponent_->GetPosition();
    const auto &areaMarkerFullColor =
            preloadColors_->
            areaMarkerColor;
    const auto &areaMarkerBorderColor = preloadColors_->
            areaMarkerBorderColor;
    const TileVector2D &startPointTileVector2D = areaMarkerComponent_->GetStartPoint();
    const TileVector2D &endPointTileVector2D = areaMarkerComponent_->GetEndPoint();
    const WorldVector2D startPoint = TileLayerComponent::TileToWorld(startPointTileVector2D);
    const WorldVector2D endPoint = TileLayerComponent::TileToWorld(endPointTileVector2D);
    int tileWidth = std::abs(endPointTileVector2D.x - startPointTileVector2D.x) + 1;
    int tileHeight = std::abs(endPointTileVector2D.y - startPointTileVector2D.y) + 1;
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
    if (cameraComponent_->IsPointInViewport(cameraPosition, rectWorldMin) ||
        cameraComponent_->IsPointInViewport(cameraPosition, rectWorldMax)) {
        CameraVector2D camMin = cameraComponent_->GetViewPortPosition(cameraPosition, rectWorldMin);
        CameraVector2D camMax = cameraComponent_->GetViewPortPosition(cameraPosition, rectWorldMax);
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

        std::string text = fmt::format(fmt::runtime(appContext_->GetLangsResources()->areaMarkerTip),
                                       tileWidth, tileHeight, tileArea);
        SDL_Surface *ttfSurface = TTF_RenderText_Blended_Wrapped(appContext_->GetFont(),
                                                                 text.c_str(), text.length(),
                                                                 appContext_->GetPreloadColors()
                                                                 ->textColor.ToSDLColor(), 0);
        if (ttfSurface == nullptr) {
            return;
        }
        const auto ttfSurfaceWidth = static_cast<float>(ttfSurface->w);
        const auto ttfSurfaceHeight = static_cast<float>(ttfSurface->h);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, ttfSurface);
        if (texture == nullptr) {
            return;
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
    AppContext::RestoreColorRenderer(renderer);
}


std::string glimmer::AreaMarkerSystem::GetName() {
    return "glimmer.AreaMarkerSystem";
}

uint8_t glimmer::AreaMarkerSystem::GetRenderOrder() {
    return RENDER_ORDER_AREA_MARKER;
}
