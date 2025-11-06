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
        Transform2DComponent, TileLayerComponent>();

    for (auto layerEntity : tileLayerEntities)
    {
        auto tileLayer = worldContext_->GetComponent<TileLayerComponent>(layerEntity->GetID());
        auto worldPosition = worldContext_->GetComponent<Transform2DComponent>(layerEntity->GetID());
        if (tileLayer == nullptr || worldPosition == nullptr)
        {
            continue;
        }

        // 视口左上角和右下角对应的tile坐标
        TileVector2D topLeftCorner = TileLayerComponent::WorldToTile(
            worldPosition->GetPosition(),
            WorldVector2D(viewportRect.x, viewportRect.y)); //Clang-Tidy: Static member accessed through instance

        TileVector2D lowerRightCorner = TileLayerComponent::WorldToTile(
            worldPosition->GetPosition(),
            WorldVector2D(viewportRect.x + viewportRect.w, viewportRect.y + viewportRect.h));

        // 计算可见区块范围（左上角对齐CHUNK_SIZE）
        auto tileToChunk = [](const int tileCoord)
        {
            return (tileCoord >= 0 ? tileCoord / CHUNK_SIZE : (tileCoord - CHUNK_SIZE + 1) / CHUNK_SIZE) * CHUNK_SIZE;
        };

        const int startChunkX = tileToChunk(topLeftCorner.x);
        const int startChunkY = tileToChunk(topLeftCorner.y);
        const int endChunkX = tileToChunk(lowerRightCorner.x);
        const int endChunkY = tileToChunk(lowerRightCorner.y);

        // 加载可见区块
        for (int cy = startChunkY; cy <= endChunkY; cy += CHUNK_SIZE)
        {
            for (int cx = startChunkX; cx <= endChunkX; cx += CHUNK_SIZE)
            {
                TileVector2D chunkPos(cx, cy);
                if (!worldContext_->ChunkIsOutOfBounds(chunkPos) && !worldContext_->HasChunk(chunkPos))
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
}

std::string glimmer::ChunkSystem::GetName()
{
    return "glimmer.ChunkSystem";
}
