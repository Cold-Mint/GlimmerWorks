//
// Created by coldmint on 2026/4/10.
//

#include "Light2DSystem.h"

#include "core/Constants.h"
#include "core/utils/ColorUtils.h"
#include "core/world/LightPropagationTraverser.h"
#include "core/world/WorldContext.h"

glimmer::Light2DSystem::Light2DSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent<TileLayerComponent>();
}

uint8_t glimmer::Light2DSystem::GetRenderOrder() {
    return RENDER_ORDER_LIGHT2D;
}

void glimmer::Light2DSystem::Render(SDL_Renderer *renderer) {
    if (worldContext_ == nullptr) {
        return;
    }
    const AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    const Config *config = appContext->GetConfig();
    if (config == nullptr) {
        return;
    }
    if (!config->light.enable) {
        return;
    }
    const auto *cameraComponent = worldContext_->GetCameraComponent();
    const auto *cameraPos = worldContext_->GetCameraTransform2D();
    if (cameraComponent == nullptr) {
        return;
    }
    if (cameraPos == nullptr) {
        return;
    }
    auto gameEntities = worldContext_->GetEntityIDWithComponents<TileLayerComponent>();
    if (gameEntities.empty()) {
        return;
    }
    auto tileLayerComponent = worldContext_->GetComponent<TileLayerComponent>(gameEntities[0]);
    if (tileLayerComponent == nullptr) {
        return;
    }
    auto viewportRect = cameraComponent->GetViewportRect(cameraPos->GetPosition());
    const float zoom = cameraComponent->GetZoom();
    const TileVector2D topLeft = TileLayerComponent::WorldToTile({viewportRect.x, viewportRect.y});
    //The purpose of adding "TILE_SIZE" in the lower right corner is to prevent blank areas from appearing.
    //右下角加TILE_SIZE的目的是，防止出现空白区域。
    const TileVector2D bottomRight = TileLayerComponent::WorldToTile({
        viewportRect.x + viewportRect.w + TILE_SIZE,
        viewportRect.y + viewportRect.h + TILE_SIZE
    });
    for (int y = topLeft.y; y <= bottomRight.y; ++y) {
        for (int x = topLeft.x; x <= bottomRight.x; ++x) {
            auto tileVector2D = TileVector2D(x, y);
            const WorldVector2D worldTilePos = TileLayerComponent::TileToWorld(tileVector2D);
            const CameraVector2D screenPos = cameraComponent->GetViewPortPosition(
                cameraPos->GetPosition(), worldTilePos);
            SDL_FRect renderQuad;
            renderQuad.w = TILE_SIZE * zoom;
            renderQuad.h = TILE_SIZE * zoom;
            renderQuad.x = screenPos.x - renderQuad.w * 0.5F;
            renderQuad.y = screenPos.y - renderQuad.h * 0.5F;
            SDL_FRect dstRect = {renderQuad.x, renderQuad.y, renderQuad.w, renderQuad.h};
            const Color *finalLightColor = worldContext_->GetFinalLightColor(tileVector2D);
            if (finalLightColor == nullptr) {
                continue;
            }
            if (finalLightColor->a == 0) {
                continue;
            }
            SDL_SetRenderDrawColor(renderer, finalLightColor->r,
                                   finalLightColor->g,
                                   finalLightColor->b,
                                   finalLightColor->a);
            SDL_RenderFillRect(renderer, &dstRect);
        }
    }
    AppContext::RestoreColorRenderer(renderer);
}

std::string glimmer::Light2DSystem::GetName() {
    return "glimmer.Light2dSystem";
}
