//
// Created by coldmint on 2026/5/22.
//

#include "BlueprintSystem.h"

#include "core/ecs/component/PlayerComponent.h"
#include "core/inventory/TileItem.h"
#include "core/world/WorldContext.h"

glimmer::BlueprintSystem::BlueprintSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent<TileLayerComponent>();
    appContext_ = worldContext->GetAppContext();
    auto gameEntities = worldContext_->GetEntityIDWithComponents<TileLayerComponent>();
    if (gameEntities.empty()) {
        return;
    }
    tileLayerComponent_ = worldContext_->GetComponent<TileLayerComponent>(gameEntities[0]);
    cameraComponent_ = worldContext_->GetCameraComponent();
    cameraTransform2DComponent_ = worldContext->GetCameraTransform2D();
    preloadColors_ = appContext_->GetPreloadColors();
    playerComponent_ = worldContext_->GetComponent<PlayerComponent>(worldContext_->GetPlayerEntity());
}

uint8_t glimmer::BlueprintSystem::GetRenderOrder() {
    return RENDER_ORDER_BLUEPRINT;
}

void glimmer::BlueprintSystem::Render(SDL_Renderer *renderer) {
    if (worldContext_ == nullptr) {
        return;
    }
    if (cameraComponent_ == nullptr) {
        return;
    }
    if (cameraTransform2DComponent_ == nullptr) {
        return;
    }
    const Tile *heldTile = nullptr;
    if (playerComponent_ != nullptr) {
        Item *item = playerComponent_->item;
        if (item != nullptr) {
            auto tileItem = dynamic_cast<TileItem *>(item);
            if (tileItem != nullptr) {
                heldTile = tileItem->GetTile();
            }
        }
    }
    const float zoom = cameraComponent_->GetZoom();
    const WorldVector2D worldTilePos = TileLayerComponent::TileToWorld(tileLayerComponent_->GetFocusPosition());
    const CameraVector2D screenPos = cameraComponent_->GetViewPortPosition(
        cameraTransform2DComponent_->GetPosition(), worldTilePos);
    SDL_FRect renderQuad;
    renderQuad.w = TILE_SIZE * zoom;
    renderQuad.h = TILE_SIZE * zoom;
    renderQuad.x = screenPos.x - renderQuad.w * 0.5F;
    renderQuad.y = screenPos.y - renderQuad.h * 0.5F;
    SDL_FRect dstRect = {renderQuad.x, renderQuad.y, renderQuad.w, renderQuad.h};
    SDL_SetRenderDrawColor(renderer, preloadColors_->game.focusTileBorderColor.r,
                           preloadColors_->game.focusTileBorderColor.g,
                           preloadColors_->game.focusTileBorderColor.b,
                           preloadColors_->game.focusTileBorderColor.a);
    SDL_RenderRect(renderer, &dstRect);
    if (heldTile != nullptr && heldTile->EnableBlueprint()) {
        SDL_Texture *blueprintTexture = heldTile->GetBlueprintTexture();
        if (blueprintTexture != nullptr) {
            Uint8 originalAlpha = 255;
            SDL_GetTextureAlphaMod(blueprintTexture, &originalAlpha);
            SDL_SetTextureAlphaMod(blueprintTexture, 128);
            SDL_RenderTexture(renderer, blueprintTexture, nullptr, &dstRect);
            SDL_SetTextureAlphaMod(blueprintTexture, originalAlpha);
        }
    }
    AppContext::RestoreColorRenderer(renderer);
}

std::string glimmer::BlueprintSystem::GetName() {
    return "BlueprintSystem";
}
