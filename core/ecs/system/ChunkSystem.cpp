//
// Created by Cold-Mint on 2025/11/4.
//

#include "ChunkSystem.h"

#include "../../Constants.h"
#include "../../world/WorldContext.h"


void glimmer::ChunkSystem::Update(float delta)
{
    const auto camera = worldContext_->GetCameraComponent();
    const auto cameraPos = worldContext_->GetCameraPosition();
    if (camera == nullptr || cameraPos == nullptr)
    {
        return;
    }
    auto viewportRect = camera->GetViewportRect(cameraPos->GetPosition());
    constexpr float chunkWorldSize = CHUNK_SIZE * TILE_SIZE;
    viewportRect.x -= preloadMargin_ * chunkWorldSize;
    viewportRect.y -= preloadMargin_ * chunkWorldSize;
    viewportRect.w += preloadMargin_ * 2 * chunkWorldSize;
    viewportRect.h += preloadMargin_ * 2 * chunkWorldSize;
    const auto tileLayerEntities = worldContext_->GetEntitiesWithComponents<
        WorldPositionComponent, TileLayerComponent>();

    for (auto layerEntity : tileLayerEntities)
    {
        auto tileLayer = worldContext_->GetComponent<TileLayerComponent>(layerEntity->GetID());
        auto worldPosition = worldContext_->GetComponent<WorldPositionComponent>(layerEntity->GetID());
        if (tileLayer == nullptr || worldPosition == nullptr)
        {
            continue;
        }

        // 视口左上角和右下角对应的tile坐标
        TileVector2D topLeftCorner = tileLayer->WorldToTile(
            worldPosition->GetPosition(),
            WorldVector2D(viewportRect.x, viewportRect.y));

        TileVector2D lowerRightCorner = tileLayer->WorldToTile(
            worldPosition->GetPosition(),
            WorldVector2D(viewportRect.x + viewportRect.w, viewportRect.y + viewportRect.h));

        // 计算可见区块范围（左上角对齐CHUNK_SIZE）
        auto tileToChunk = [](int tileCoord) {
            return (tileCoord >= 0 ? tileCoord / CHUNK_SIZE : (tileCoord - CHUNK_SIZE + 1) / CHUNK_SIZE) * CHUNK_SIZE;
        };

        int startChunkX = tileToChunk(topLeftCorner.x);
        int startChunkY = tileToChunk(topLeftCorner.y);
        int endChunkX = tileToChunk(lowerRightCorner.x);
        int endChunkY = tileToChunk(lowerRightCorner.y);

        // 加载可见区块
        for (int cy = startChunkY; cy <= endChunkY; cy += CHUNK_SIZE)
        {
            for (int cx = startChunkX; cx <= endChunkX; cx += CHUNK_SIZE)
            {
                TileVector2D chunkPos(cx, cy);
                if (!worldContext_->HasChunk(chunkPos))
                {
                    worldContext_->LoadChunkAt(tileLayer, chunkPos);
                }
            }
        }

        // 卸载不可见区块
        std::vector<TileVector2D> chunksToUnload;
        for (const auto& loadedPos : worldContext_->GetAllChunks() | std::views::keys)
        {
            if (loadedPos.x < startChunkX || loadedPos.x > endChunkX ||
                loadedPos.y < startChunkY || loadedPos.y > endChunkY)
            {
                chunksToUnload.push_back(loadedPos);
            }
        }

        for (const auto& pos : chunksToUnload)
        {
            worldContext_->UnloadChunkAt(tileLayer, pos);
        }
    }
    for (const auto& kv : worldContext_->GetAllChunks())
    {
        const TileVector2D& chunkPos = kv.first;
        std::cout << "Chunk key: x=" << chunkPos.x << " y=" << chunkPos.y << std::endl;
    }
}

std::string glimmer::ChunkSystem::GetName()
{
    return "glimmer.ChunkSystem";
}
