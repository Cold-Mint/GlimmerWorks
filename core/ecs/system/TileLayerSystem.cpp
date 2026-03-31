//
// Created by Cold-Mint on 2025/11/2.
//

#include "TileLayerSystem.h"

#include "../../Constants.h"
#include "../component/CameraComponent.h"
#include "../component/TileLayerComponent.h"
#include "../component/Transform2DComponent.h"
#include "../../world/WorldContext.h"
#include "core/world/Tile.h"

glimmer::TileLayerSystem::TileLayerSystem(WorldContext *worldContext)
    : GameSystem(worldContext) {
    RequireComponent<TileLayerComponent>();
}

void glimmer::TileLayerSystem::Render(SDL_Renderer *renderer) {
    if (worldContext_ == nullptr) {
        return;
    }
    const AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr) {
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
    for (auto entity: gameEntities) {
        auto tileLayerComponent = worldContext_->GetComponent<TileLayerComponent>(entity);
        if (tileLayerComponent == nullptr) {
            continue;
        }
        auto viewportRect = cameraComponent->GetViewportRect(cameraPos->GetPosition());
        const float zoom = cameraComponent->GetZoom();

        std::vector<std::pair<TileVector2D, Tile *> > visibleTiles =
                tileLayerComponent->GetTilesInViewport(viewportRect);
        TileVector2D focusPosition = tileLayerComponent->GetFocusPosition();
        for (const auto &[tileCoord, tile]: visibleTiles) {
            const WorldVector2D worldTilePos = TileLayerComponent::TileToWorld(tileCoord);
            const CameraVector2D screenPos = cameraComponent->GetViewPortPosition(
                cameraPos->GetPosition(), worldTilePos);
            SDL_FRect renderQuad;
            renderQuad.w = TILE_SIZE * zoom;
            renderQuad.h = TILE_SIZE * zoom;
            renderQuad.x = screenPos.x - renderQuad.w * 0.5F;
            renderQuad.y = screenPos.y - renderQuad.h * 0.5F;
            SDL_FRect dstRect = {renderQuad.x, renderQuad.y, renderQuad.w, renderQuad.h};
            if (!SDL_RenderTexture(renderer, tile->GetTexture(), nullptr, &dstRect)) {
                LogCat::e("SDL_RenderTexture Error: ", SDL_GetError());
            }
            if (tileCoord == focusPosition) {
                SDL_SetRenderDrawColor(renderer, appContext->GetPreloadColors()->game.focusTileBorderColor.r,
                                       appContext->GetPreloadColors()->game.focusTileBorderColor.g,
                                       appContext->GetPreloadColors()->game.focusTileBorderColor.b,
                                       appContext->GetPreloadColors()->game.focusTileBorderColor.a);
                SDL_RenderRect(renderer, &dstRect);
            }
        }
    }
    AppContext::RestoreColorRenderer(renderer);
}

uint8_t glimmer::TileLayerSystem::GetRenderOrder() {
    return RENDER_ORDER_TILE_LAYER;
}

std::string glimmer::TileLayerSystem::GetName() {
    return "glimmer.TileLayerSystem";
}
