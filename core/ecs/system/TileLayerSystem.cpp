//
// Created by Cold-Mint on 2025/11/2.
//

#include "TileLayerSystem.h"

#include "../../Constants.h"
#include "../component/CameraComponent.h"
#include "../component/TileLayerComponent.h"
#include "../component/WorldPositionComponent.h"
#include "../../world/WorldContext.h"

void glimmer::TileLayerSystem::Render(SDL_Renderer* renderer)
{
    auto cameraComponent = worldContext_->GetCameraComponent();
    auto cameraPos = worldContext_->GetCameraPosition();
    if (cameraComponent == nullptr)
    {
        return;
    }
    if (cameraPos == nullptr)
    {
        return;
    }
    auto entitys = worldContext_->GetEntitiesWithComponents<TileLayerComponent, WorldPositionComponent>();
    for (auto entity : entitys)
    {
        auto tileLayerComponent = worldContext_->GetComponent<TileLayerComponent>(entity->GetID());
        if (tileLayerComponent != nullptr)
        {
            auto worldPositionComponent = worldContext_->GetComponent<WorldPositionComponent>(entity->GetID());
            if (worldPositionComponent != nullptr)
            {
                auto viewportRect = cameraComponent->GetViewportRect(cameraPos->GetPosition());
                const auto layerWorldPos = worldPositionComponent->GetPosition();
                const float zoom = cameraComponent->GetZoom();

                std::vector<std::pair<TileVector2D, Tile>> visibleTiles =
                    tileLayerComponent->GetTilesInViewport(layerWorldPos, viewportRect);
                SDL_Color oldColor;
                SDL_GetRenderDrawColor(renderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);

                for (const auto& [tileCoord, tile] : visibleTiles)
                {
                    WorldVector2D worldTilePos = TileLayerComponent::TileToWorld(layerWorldPos, tileCoord);

                    CameraVector2D screenPos = cameraComponent->GetViewPortPosition(
                        cameraPos->GetPosition(), worldTilePos);

                    SDL_FRect renderQuad;
                    renderQuad.w = TILE_SIZE * zoom;
                    renderQuad.h = TILE_SIZE * zoom;

                    renderQuad.x = screenPos.x - renderQuad.w * 0.5f;
                    renderQuad.y = screenPos.y - renderQuad.h * 0.5f;

                    SDL_SetRenderDrawColor(renderer, tile.color.r, tile.color.g, tile.color.b, tile.color.a);
                    SDL_RenderFillRect(renderer, &renderQuad);
                }

                SDL_SetRenderDrawColor(renderer, oldColor.r, oldColor.g, oldColor.b, oldColor.a);
            }
        }
    }
}

uint8_t glimmer::TileLayerSystem::GetRenderOrder()
{
    return RENDER_ORDER_TILE_LAYER;
}

std::string glimmer::TileLayerSystem::GetName()
{
    return "glimmer.TileLayerSystem";
}
