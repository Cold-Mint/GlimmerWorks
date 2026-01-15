//
// Created by Cold-Mint on 2025/11/4.
//

#include "ChunkSystem.h"

#include "../../Constants.h"
#include "../../world/WorldContext.h"
#include "../component/Transform2DComponent.h"


void glimmer::ChunkSystem::Update(float delta) {
    const auto *camera = worldContext_->GetCameraComponent();
    const auto *cameraPos = worldContext_->GetCameraTransform2D();
    if (camera == nullptr || cameraPos == nullptr) {
        return;
    }
    auto viewportRect = camera->GetViewportRect(cameraPos->GetPosition());
    constexpr float chunkWorldSize = CHUNK_SIZE * TILE_SIZE;
    viewportRect.x -= preloadMargin_ * chunkWorldSize;
    viewportRect.y -= preloadMargin_ * chunkWorldSize;
    viewportRect.w += preloadMargin_ * 2 * chunkWorldSize;
    viewportRect.h += preloadMargin_ * 2 * chunkWorldSize;
    const auto tileLayerEntities = worldContext_->GetEntitiesWithComponents<
        TileLayerComponent>();

    for (auto layerEntity: tileLayerEntities) {
        auto tileLayer = worldContext_->GetComponent<TileLayerComponent>(layerEntity->GetID());
        if (tileLayer == nullptr) {
            continue;
        }

        // 视口左上角和右下角对应的tile坐标
        TileVector2D topLeftCorner = TileLayerComponent::WorldToTile(
            WorldVector2D(viewportRect.x, viewportRect.y));

        TileVector2D lowerRightCorner = TileLayerComponent::WorldToTile(
            WorldVector2D(viewportRect.x + viewportRect.w, viewportRect.y + viewportRect.h));
        const TileVector2D startChunk = Chunk::TileCoordinatesToChunkVertexCoordinates(topLeftCorner);
        const TileVector2D endChunk = Chunk::TileCoordinatesToChunkVertexCoordinates(lowerRightCorner);

        // 加载可见区块
        for (int cy = startChunk.y; cy <= endChunk.y; cy += CHUNK_SIZE) {
            for (int cx = startChunk.x; cx <= endChunk.x; cx += CHUNK_SIZE) {
                TileVector2D chunkPos(cx, cy);
                if (!WorldContext::ChunkIsOutOfBounds(chunkPos) && !worldContext_->HasChunk(chunkPos)) {
                    worldContext_->LoadChunkAt(chunkPos);
                }
            }
        }

        // 卸载不可见区块
        std::vector<TileVector2D> chunksToUnload;
        std::unordered_map<TileVector2D, Chunk *, Vector2DIHash> allChunks = *worldContext_->
                GetAllChunks();
        for (const
             auto &loadedPos: allChunks | std::views::keys) {
            if (loadedPos.x < startChunk.x || loadedPos.x >
                endChunk.x ||
                loadedPos.y < startChunk.y || loadedPos.y >
                endChunk.y
            ) {
                chunksToUnload.push_back(loadedPos);
            }
        }

        for (const auto &pos: chunksToUnload) {
            worldContext_->UnloadChunkAt(pos);
        }
    }
}

std::string glimmer::ChunkSystem::GetName() {
    return "glimmer.ChunkSystem";
}
