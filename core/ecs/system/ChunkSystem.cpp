//
// Created by Cold-Mint on 2025/11/4.
//

#include "ChunkSystem.h"

#include "../../Constants.h"
#include "../../world/WorldContext.h"
#include "../component/Transform2DComponent.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/ecs/component/CameraComponent.h"
#include "core/world/generator/Chunk.h"


glimmer::ChunkSystem::ChunkSystem(WorldContext *worldContext)
    : GameSystem(worldContext) {
    RequireComponent<TileLayerComponent>();
    RequireComponent<CameraComponent>();
    RequireComponent<PlayerComponent>();
}

void glimmer::ChunkSystem::Update(float delta) {
    const auto *camera = worldContext_->GetCameraComponent();
    const auto *cameraPos = worldContext_->GetCameraTransform2D();
    if (camera == nullptr || cameraPos == nullptr) {
        return;
    }
    const auto originalViewportRect = camera->GetViewportRect(cameraPos->GetPosition());
    constexpr float chunkWorldSize = CHUNK_SIZE * TILE_SIZE;
    AppContext *appContext = worldContext_->GetAppContext();
    Config *config = appContext->GetConfig();
    //Pre-calculated terrain range.
    //预计算的地形范围。
    auto preloadedTerrainViewportRect = originalViewportRect;
    preloadedTerrainViewportRect.x -= config->world.preloadStructureRadius * chunkWorldSize;
    preloadedTerrainViewportRect.y -= config->world.preloadStructureRadius * chunkWorldSize;
    preloadedTerrainViewportRect.w += config->world.preloadStructureRadius * 2 * chunkWorldSize;
    preloadedTerrainViewportRect.h += config->world.preloadStructureRadius * 2 * chunkWorldSize;
    //The complete range of pre-calculated blocks.
    //预计算的完整区块范围。
    auto preloadedChunkViewportRect = originalViewportRect;
    preloadedChunkViewportRect.x -= config->world.preloadChunkRadius * chunkWorldSize;
    preloadedChunkViewportRect.y -= config->world.preloadChunkRadius * chunkWorldSize;
    preloadedChunkViewportRect.w += config->world.preloadChunkRadius * 2 * chunkWorldSize;
    preloadedChunkViewportRect.h += config->world.preloadChunkRadius * 2 * chunkWorldSize;
    const auto tileLayerEntities = worldContext_->GetEntityIDWithComponents<
        TileLayerComponent>();

    for (auto layerEntity: tileLayerEntities) {
        auto tileLayer = worldContext_->GetComponent<TileLayerComponent>(layerEntity);
        if (tileLayer == nullptr) {
            continue;
        }

        TileVector2D topLeftTerrainCorner = TileLayerComponent::WorldToTile(
            WorldVector2D(preloadedTerrainViewportRect.x, preloadedTerrainViewportRect.y));

        TileVector2D lowerRightTerrainCorner = TileLayerComponent::WorldToTile(
            WorldVector2D(preloadedTerrainViewportRect.x + preloadedTerrainViewportRect.w,
                          preloadedTerrainViewportRect.y + preloadedTerrainViewportRect.h));
        const TileVector2D startTerrain = Chunk::TileCoordinatesToChunkVertexCoordinates(topLeftTerrainCorner);
        const TileVector2D endTerrain = Chunk::TileCoordinatesToChunkVertexCoordinates(lowerRightTerrainCorner);
        //Pre-calculate the terrain
        //预先计算地形
        for (int cy = startTerrain.y; cy < endTerrain.y; cy += CHUNK_SIZE) {
            for (int cx = startTerrain.x; cx < endTerrain.x; cx += CHUNK_SIZE) {
                TileVector2D chunkPos(cx, cy);
                if (!WorldContext::ChunkIsOutOfBounds(chunkPos)) {
                    worldContext_->LoadTerrainAt(chunkPos);
                }
            }
        }

        TileVector2D topLeftChunkCorner = TileLayerComponent::WorldToTile(
            WorldVector2D(preloadedChunkViewportRect.x, preloadedChunkViewportRect.y));

        TileVector2D lowerRightChunkCorner = TileLayerComponent::WorldToTile(
            WorldVector2D(preloadedChunkViewportRect.x + preloadedChunkViewportRect.w,
                          preloadedChunkViewportRect.y + preloadedChunkViewportRect.h));
        const TileVector2D startChunk = Chunk::TileCoordinatesToChunkVertexCoordinates(topLeftChunkCorner);
        const TileVector2D endChunk = Chunk::TileCoordinatesToChunkVertexCoordinates(lowerRightChunkCorner);

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
        std::unordered_map<TileVector2D, Chunk *, Vector2DIHash> &allChunks = *worldContext_->
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

        std::vector<TileVector2D> terrainToUnload;
        std::unordered_map<TileVector2D, TerrainResult *, Vector2DIHash> &terrain = *worldContext_->GetTerrainResults();
        for (const
             auto &loadedPos: terrain | std::views::keys) {
            if (loadedPos.x < startTerrain.x || loadedPos.x >
                endTerrain.x ||
                loadedPos.y < startTerrain.y || loadedPos.y >
                endTerrain.y
            ) {
                terrainToUnload.push_back(loadedPos);
            }
        }
        for (const auto &pos: terrainToUnload) {
            worldContext_->UnloadTerrainAt(pos);
        }
    }
}

std::string glimmer::ChunkSystem::GetName() {
    return "glimmer.ChunkSystem";
}
