//
// Created by coldmint on 2026/4/10.
//

#include "Light2DSystem.h"

#include "core/Constants.h"
#include "core/world/WorldContext.h"


glimmer::Light2DSystem::Light2DSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent<TileLayerComponent>();
}

void glimmer::Light2DSystem::Update(float delta) {
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
    CameraComponent *cameraComponent = worldContext_->GetCameraComponent();
    if (cameraComponent == nullptr) {
        return;
    }
    Transform2DComponent *cameraTransform2DComponent = worldContext_->GetCameraTransform2D();
    if (cameraTransform2DComponent == nullptr) {
        return;
    }
    SDL_FRect viewRect = cameraComponent->GetViewportRect(cameraTransform2DComponent->GetPosition());
    //The complete range of pre-calculated blocks.
    //预计算的完整区块范围。
    auto preloadedLightingViewportRect = viewRect;
    const float preloadLightingRadius = config->world.preloadLightingRadius;
    constexpr float chunkWorldSize = CHUNK_SIZE * TILE_SIZE;
    preloadedLightingViewportRect.x -= preloadLightingRadius * chunkWorldSize;
    preloadedLightingViewportRect.y -= preloadLightingRadius * chunkWorldSize;
    preloadedLightingViewportRect.w += preloadLightingRadius * 2 * chunkWorldSize;
    preloadedLightingViewportRect.h += preloadLightingRadius * 2 * chunkWorldSize;
    TileVector2D topLeftLightingCorner = TileLayerComponent::WorldToTile(
        WorldVector2D(preloadedLightingViewportRect.x, preloadedLightingViewportRect.y));
    TileVector2D lowerRightLightingCorner = TileLayerComponent::WorldToTile(
        WorldVector2D(preloadedLightingViewportRect.x + preloadedLightingViewportRect.w,
                      preloadedLightingViewportRect.y + preloadedLightingViewportRect.h));
    const TileVector2D startChunk = Chunk::TileCoordinatesToChunkVertexCoordinates(topLeftLightingCorner);
    const TileVector2D endChunk = Chunk::TileCoordinatesToChunkVertexCoordinates(lowerRightLightingCorner);
    for (int cy = startChunk.y; cy <= endChunk.y; cy += CHUNK_SIZE) {
        for (int cx = startChunk.x; cx <= endChunk.x; cx += CHUNK_SIZE) {
            TileVector2D chunkPos(cx, cy);
            if (!WorldContext::ChunkIsOutOfBounds(chunkPos) && worldContext_->HasChunk(chunkPos)) {
                Chunk *chunk = worldContext_->GetChunk(chunkPos);
                if (chunk == nullptr) {
                    continue;
                }
                for (int y = 0; y < CHUNK_SIZE; y++) {
                    for (int x = 0; x < CHUNK_SIZE; x++) {
                        std::vector<Tile *> tiles = chunk->GetTopVisibleTiles(
                            Ground | BackGround, TileVector2D{x, y});
                        if (tiles.empty()) {
                            continue;
                        }
                        for (auto &tile: tiles) {
                            //Restore the color of the tiles.
                            //恢复瓦片的颜色。
                            tile->SetLightColor(tile->GetEmissionColor());
                        }
                    }
                }
            }
        }
    }
}

std::string glimmer::Light2DSystem::GetName() {
    return "glimmer.Light2dSystem";
}
