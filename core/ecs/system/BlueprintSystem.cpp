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
    const GameEntity::ID playerEntity = worldContext_->GetPlayerEntity();
    playerComponent_ = worldContext_->GetComponent<PlayerComponent>(playerEntity);
    playerTransform2DComponent_ = worldContext_->GetComponent<Transform2DComponent>(playerEntity);
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
    if (playerComponent_ == nullptr) {
        return;
    }
    if (playerTransform2DComponent_ == nullptr) {
        return;
    }
    const Tile *heldTile = nullptr;
    uint8_t tileWidth = 1;
    uint8_t tileHeight = 1;
    TileVector2D tileAnchor = {0, 0};
    const WorldVector2D focusWorldTilePos = TileLayerComponent::TileToWorld(tileLayerComponent_->GetFocusPosition());

    Item *item = playerComponent_->item;
    if (item != nullptr) {
        auto tileItem = dynamic_cast<TileItem *>(item);
        if (tileItem != nullptr) {
            heldTile = tileItem->GetTile();
            if (heldTile != nullptr) {
                tileWidth = heldTile->GetTileWidth();
                tileHeight = heldTile->GetTileHeight();
                tileAnchor = *heldTile->GetTileAnchor();
                WorldVector2D playerPosition = playerTransform2DComponent_->GetPosition();
                bool atLeft = focusWorldTilePos.x < playerPosition.x;
                if (atLeft && heldTile->IsAllowDirAdjustAnchor()) {
                    tileAnchor.x = tileWidth - 1 - tileAnchor.x;
                }
            }
        }
    }
    const float zoom = cameraComponent_->GetZoom();
    const CameraVector2D screenPos = cameraComponent_->GetViewPortPosition(
        cameraTransform2DComponent_->GetPosition(), focusWorldTilePos);
    SDL_FRect renderQuad;
    renderQuad.w = static_cast<float>(tileWidth) * TILE_SIZE * zoom;
    renderQuad.h = static_cast<float>(tileHeight) * TILE_SIZE * zoom;
    renderQuad.x = screenPos.x - HALF_TILE_SIZE - static_cast<float>(tileAnchor.x) * TILE_SIZE * zoom;
    renderQuad.y = screenPos.y - HALF_TILE_SIZE - static_cast<float>(tileAnchor.y) * TILE_SIZE * zoom;
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
