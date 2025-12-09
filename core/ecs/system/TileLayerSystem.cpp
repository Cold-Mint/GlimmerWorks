//
// Created by Cold-Mint on 2025/11/2.
//

#include "TileLayerSystem.h"

#include "../../Constants.h"
#include "../component/CameraComponent.h"
#include "../component/TileLayerComponent.h"
#include "../component/Transform2DComponent.h"
#include "../../world/WorldContext.h"

void glimmer::TileLayerSystem::Render(SDL_Renderer *renderer) {
    const auto *cameraComponent = worldContext_->GetCameraComponent();
    const auto *cameraPos = worldContext_->GetCameraTransform2D();
    if (cameraComponent == nullptr) {
        return;
    }
    if (cameraPos == nullptr) {
        return;
    }
    auto gameEntities = worldContext_->GetEntitiesWithComponents<TileLayerComponent, Transform2DComponent>();
    for (auto entity: gameEntities) {
        auto tileLayerComponent = worldContext_->GetComponent<TileLayerComponent>(entity->GetID());
        if (tileLayerComponent != nullptr) {
            const auto *worldPositionComponent = worldContext_->GetComponent<Transform2DComponent>(entity->GetID());
            if (worldPositionComponent != nullptr) {
                auto viewportRect = cameraComponent->GetViewportRect(cameraPos->GetPosition());
                const auto layerWorldPos = worldPositionComponent->GetPosition();
                const float zoom = cameraComponent->GetZoom();

                std::vector<std::pair<TileVector2D, Tile> > visibleTiles =
                        tileLayerComponent->GetTilesInViewport(layerWorldPos, viewportRect);
                SDL_Color oldColor;
                SDL_GetRenderDrawColor(renderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);

                for (const auto &[tileCoord, tile]: visibleTiles) {
                    WorldVector2D worldTilePos = TileLayerComponent::TileToWorld(layerWorldPos, tileCoord);

                    CameraVector2D screenPos = cameraComponent->GetViewPortPosition(
                        cameraPos->GetPosition(), worldTilePos);

                    SDL_FRect renderQuad;
                    renderQuad.w = TILE_SIZE * zoom;
                    renderQuad.h = TILE_SIZE * zoom;

                    renderQuad.x = screenPos.x - renderQuad.w * 0.5f;
                    renderQuad.y = screenPos.y - renderQuad.h * 0.5f;
                    if (appContext_->GetConfig()->debug.onlyDrawHumidity) {
                        // draw humidity color
                        SDL_Color newColor;
                        newColor.r = 0;
                        newColor.g = static_cast<uint8_t>(255 * tile.humidity);
                        newColor.b = 0;
                        newColor.a = 255;
                        SDL_SetRenderDrawColor(renderer, newColor.r, newColor.g, newColor.b, newColor.a);

                        SDL_RenderFillRect(renderer, &renderQuad);
                    } else {
                        SDL_FRect dstRect = {renderQuad.x, renderQuad.y, renderQuad.w, renderQuad.h};
                        if (!SDL_RenderTexture(renderer, tile.texture.get(), nullptr, &dstRect)) {
                            LogCat::e("SDL_RenderTexture Error: ", SDL_GetError());
                        }
                    }
                }

                SDL_SetRenderDrawColor(renderer, oldColor.r, oldColor.g, oldColor.b, oldColor.a);
            }
        }
    }
}

uint8_t glimmer::TileLayerSystem::GetRenderOrder() {
    return RENDER_ORDER_TILE_LAYER;
}

std::string glimmer::TileLayerSystem::GetName() {
    return "glimmer.TileLayerSystem";
}
