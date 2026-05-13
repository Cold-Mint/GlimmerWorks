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

void glimmer::ChunkSystem::Update(const float delta) {
    if (!chunkTasks_.empty()) {
        for (int i = 0; i < 4; i++) {
            if (chunkTasks_.empty()) {
                break;
            }
            ChunkTask chunkTask = chunkTasks_.front();
            uint64_t id = chunkTask.GetId();
            taskIdSet_.erase(id);
            chunkTasks_.pop_front();
            if (chunkTask.chunkType == LoadTerrain) {
                worldContext_->LoadTerrainAt(chunkTask.tileVector2D);
            }
            if (chunkTask.chunkType == LoadChunk) {
                worldContext_->LoadChunkAt(chunkTask.tileVector2D);
            }
            if (chunkTask.chunkType == UnloadChunk) {
                worldContext_->UnloadChunkAt(chunkTask.tileVector2D);
            }
            if (chunkTask.chunkType == UnloadTerrain) {
                worldContext_->UnloadTerrainAt(chunkTask.tileVector2D);
            }
        }
    }
    const AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    const Config *config = appContext->GetConfig();
    if (config == nullptr) {
        return;
    }
    accumTime_ += delta;
    if (!firstTime_ && accumTime_ < config->world.chunkSpawnCleanInterval) {
        return;
    }
    firstTime_ = false;
    accumTime_ -= config->world.chunkSpawnCleanInterval;
    const auto *camera = worldContext_->GetCameraComponent();
    const auto *cameraTransform2D = worldContext_->GetCameraTransform2D();
    if (camera == nullptr || cameraTransform2D == nullptr) {
        return;
    }
    WorldVector2D cameraPosition = cameraTransform2D->GetPosition();
    if (cameraPosition_.x == cameraPosition.x && cameraPosition_.y == cameraPosition.y) {
        return;
    }
    cameraPosition_.x = cameraPosition.x;
    cameraPosition_.y = cameraPosition.y;
    const auto originalViewportRect = camera->GetViewportRect(cameraPosition);
    constexpr float chunkWorldSize = CHUNK_SIZE * TILE_SIZE;
    //Pre-calculated terrain range.
    //预计算的地形范围。
    auto preloadedTerrainViewportRect = originalViewportRect;
    const float preloadStructureRadius = config->world.preloadStructureRadius;
    preloadedTerrainViewportRect.x -= preloadStructureRadius * chunkWorldSize;
    preloadedTerrainViewportRect.y -= preloadStructureRadius * chunkWorldSize;
    preloadedTerrainViewportRect.w += preloadStructureRadius * 2 * chunkWorldSize;
    preloadedTerrainViewportRect.h += preloadStructureRadius * 2 * chunkWorldSize;
    //The complete range of pre-calculated blocks.
    //预计算的完整区块范围。
    auto preloadedChunkViewportRect = originalViewportRect;
    const float preloadChunkRadius = config->world.preloadChunkRadius;
    preloadedChunkViewportRect.x -= preloadChunkRadius * chunkWorldSize;
    preloadedChunkViewportRect.y -= preloadChunkRadius * chunkWorldSize;
    preloadedChunkViewportRect.w += preloadChunkRadius * 2 * chunkWorldSize;
    preloadedChunkViewportRect.h += preloadChunkRadius * 2 * chunkWorldSize;

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
                ChunkTask chunkTask;
                chunkTask.chunkType = LoadTerrain;
                chunkTask.tileVector2D = chunkPos;
                uint64_t id = chunkTask.GetId();
                if (!taskIdSet_.contains(id)) {
                    chunkTasks_.push_back(chunkTask);
                    taskIdSet_.insert(id);
                }
            }
        }
    }

    const TileVector2D topLeftChunkCorner = TileLayerComponent::WorldToTile(
        WorldVector2D(preloadedChunkViewportRect.x, preloadedChunkViewportRect.y));

    const TileVector2D lowerRightChunkCorner = TileLayerComponent::WorldToTile(
        WorldVector2D(preloadedChunkViewportRect.x + preloadedChunkViewportRect.w,
                      preloadedChunkViewportRect.y + preloadedChunkViewportRect.h));
    const TileVector2D startChunk = Chunk::TileCoordinatesToChunkVertexCoordinates(topLeftChunkCorner);
    const TileVector2D endChunk = Chunk::TileCoordinatesToChunkVertexCoordinates(lowerRightChunkCorner);

    // 加载可见区块
    for (int cy = startChunk.y; cy <= endChunk.y; cy += CHUNK_SIZE) {
        for (int cx = startChunk.x; cx <= endChunk.x; cx += CHUNK_SIZE) {
            TileVector2D chunkPos(cx, cy);
            if (!WorldContext::ChunkIsOutOfBounds(chunkPos) && !worldContext_->HasChunk(chunkPos)) {
                ChunkTask chunkTask;
                chunkTask.chunkType = LoadChunk;
                chunkTask.tileVector2D = chunkPos;
                uint64_t id = chunkTask.GetId();
                if (!taskIdSet_.contains(id)) {
                    chunkTasks_.push_back(chunkTask);
                    taskIdSet_.insert(id);
                }
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
        ChunkTask chunkTask;
        chunkTask.chunkType = UnloadChunk;
        chunkTask.tileVector2D = pos;
        uint64_t id = chunkTask.GetId();
        if (!taskIdSet_.contains(id)) {
            chunkTasks_.push_back(chunkTask);
            taskIdSet_.insert(id);
        }
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
        ChunkTask chunkTask;
        chunkTask.chunkType = UnloadTerrain;
        chunkTask.tileVector2D = pos;
        uint64_t id = chunkTask.GetId();
        if (!taskIdSet_.contains(id)) {
            chunkTasks_.push_back(chunkTask);
            taskIdSet_.insert(id);
        }
    }
}

std::string glimmer::ChunkSystem::GetName() {
    return "glimmer.ChunkSystem";
}
