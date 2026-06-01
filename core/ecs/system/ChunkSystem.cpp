/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "ChunkSystem.h"

#include "../../Constants.h"
#include "../../world/WorldContext.h"
#include "../component/Transform2DComponent.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/ecs/component/CameraComponent.h"
#include "core/world/generator/Chunk.h"


void glimmer::ChunkSystem::ExecuteLoadTerrainTask(const uint16_t loadTerrainBatch) {
    if (loadTerrainTasks_.empty()) {
        return;
    }
    if (loadTerrainBatch == 0) {
        while (!loadTerrainTasks_.empty()) {
            auto task = std::move(loadTerrainTasks_.back());
            taskFingerprintSet_.erase(task->GetFingerprint());
            worldContext_->LoadTerrainAt(task->GetChunkVertexCoordinates());
            loadTerrainTasks_.pop_back();
        }
    } else {
        for (uint16_t i = 0; i < loadTerrainBatch; i++) {
            if (loadTerrainTasks_.empty()) {
                break;
            }
            auto task = std::move(loadTerrainTasks_.back());
            taskFingerprintSet_.erase(task->GetFingerprint());
            worldContext_->LoadTerrainAt(task->GetChunkVertexCoordinates());
            loadTerrainTasks_.pop_back();
        }
    }
}

void glimmer::ChunkSystem::ExecuteLoadChunkTask(const uint16_t loadChunkBatch) {
    if (loadChunkTasks_.empty()) {
        return;
    }

    if (loadChunkBatch == 0) {
        while (!loadChunkTasks_.empty()) {
            auto task = std::move(loadChunkTasks_.back());
            taskFingerprintSet_.erase(task->GetFingerprint());
            worldContext_->LoadChunkAt(task->GetChunkVertexCoordinates());
            loadChunkTasks_.pop_back();
        }
    } else {
        for (uint16_t i = 0; i < loadChunkBatch; i++) {
            if (loadChunkTasks_.empty()) {
                break;
            }
            auto task = std::move(loadChunkTasks_.back());
            taskFingerprintSet_.erase(task->GetFingerprint());
            worldContext_->LoadChunkAt(task->GetChunkVertexCoordinates());
            loadChunkTasks_.pop_back();
        }
    }
}

void glimmer::ChunkSystem::ExecuteUnloadChunkTask(const uint16_t unloadChunkBatch) {
    if (unloadChunkTasks_.empty()) {
        return;
    }
    if (unloadChunkBatch == 0) {
        while (!unloadChunkTasks_.empty()) {
            auto task = std::move(unloadChunkTasks_.back());
            taskFingerprintSet_.erase(task->GetFingerprint());
            worldContext_->UnloadChunkAt(task->GetChunkVertexCoordinates());
            unloadChunkTasks_.pop_back();
        }
    } else {
        for (uint16_t i = 0; i < unloadChunkBatch; i++) {
            if (unloadChunkTasks_.empty()) {
                break;
            }
            auto task = std::move(unloadChunkTasks_.back());
            taskFingerprintSet_.erase(task->GetFingerprint());
            worldContext_->UnloadChunkAt(task->GetChunkVertexCoordinates());
            unloadChunkTasks_.pop_back();
        }
    }
}

void glimmer::ChunkSystem::ExecuteUnloadTerrainTask(const uint16_t unloadTerrainBatch) {
    if (unloadTerrainTasks_.empty()) {
        return;
    }
    if (unloadTerrainBatch == 0) {
        while (!unloadTerrainTasks_.empty()) {
            auto task = std::move(unloadTerrainTasks_.back());
            taskFingerprintSet_.erase(task->GetFingerprint());
            worldContext_->UnloadTerrainAt(task->GetChunkVertexCoordinates());
            unloadTerrainTasks_.pop_back();
        }
    } else {
        for (uint16_t i = 0; i < unloadTerrainBatch; i++) {
            if (unloadTerrainTasks_.empty()) {
                break;
            }
            auto task = std::move(unloadTerrainTasks_.back());
            taskFingerprintSet_.erase(task->GetFingerprint());
            worldContext_->UnloadTerrainAt(task->GetChunkVertexCoordinates());
            unloadTerrainTasks_.pop_back();
        }
    }
}

void glimmer::ChunkSystem::PushTask(std::vector<std::unique_ptr<ChunkTask> > &taskList,
                                    std::unique_ptr<ChunkTask> chunkTask, const uint64_t fingerprint) {
    taskList.push_back(std::move(chunkTask));
    taskFingerprintSet_.insert(fingerprint);
}

void glimmer::ChunkSystem::SetOriginAndSort(std::vector<std::unique_ptr<ChunkTask> > &taskList, TileVector2D origin,
                                            bool sortAscending) {
    for (const auto &task: taskList) {
        task->SetOrigin(origin);
    }
    std::sort(taskList.begin(), taskList.end(), [sortAscending](const auto &a, const auto &b) {
        const auto distA = a->GetDistance();
        const auto distB = b->GetDistance();
        return sortAscending ? distA < distB : distA > distB;
    });
}

glimmer::ChunkSystem::ChunkSystem(WorldContext *worldContext)
    : GameSystem(worldContext) {
    RequireComponent(COMPONENT_TILE_LAYER);
    RequireComponent(COMPONENT_CAMERA);
    RequireComponent(COMPONENT_PLAYER);
}

void glimmer::ChunkSystem::Update(const float delta) {
    if (worldContext_ == nullptr) {
        return;
    }
    const auto *cameraComponent = worldContext_->GetCameraComponent();
    if (cameraComponent == nullptr) {
        return;
    }
    const auto *cameraTransform2D = worldContext_->GetCameraTransform2D();
    if (cameraTransform2D == nullptr) {
        return;
    }
    constexpr float chunkWorldSize = CHUNK_SIZE * TILE_SIZE;
    auto viewportRect = cameraComponent->GetViewportRect(cameraTransform2D->GetPosition());
    const TileVector2D topLeft = TileLayerComponent::WorldToTile({
        viewportRect.x - chunkWorldSize, viewportRect.y - chunkWorldSize
    });
    const TileVector2D bottomRight = TileLayerComponent::WorldToTile({
        viewportRect.x + viewportRect.w + chunkWorldSize,
        viewportRect.y + viewportRect.h + chunkWorldSize
    });
    for (int x = topLeft.x; x < bottomRight.x; x += CHUNK_SIZE) {
        for (int y = topLeft.y; y < bottomRight.y; y += CHUNK_SIZE) {
            Chunk *chunk = worldContext_->GetChunk(Chunk::TileCoordinatesToChunkVertexCoordinates(TileVector2D(x, y)));
            if (chunk == nullptr) {
                continue;
            }
            chunk->UpdateFadeInAnimation(delta);
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
    const float loadTerrainInterval = config->world.loadTerrainInterval;
    if (loadTerrainInterval == 0.0F) {
        ExecuteLoadTerrainTask(config->world.loadTerrainBatch);
    } else {
        loadTerrainAccumTime_ += delta;
        if (loadTerrainAccumTime_ > loadTerrainInterval) {
            ExecuteLoadTerrainTask(config->world.loadTerrainBatch);
            loadTerrainAccumTime_ -= loadTerrainInterval;
        }
    }

    const float loadChunkInterval = config->world.loadChunkInterval;
    if (loadChunkInterval == 0.0F) {
        ExecuteLoadChunkTask(config->world.loadChunkBatch);
    } else {
        loadChunkAccumTime_ += delta;
        if (loadChunkAccumTime_ > loadChunkInterval) {
            ExecuteLoadChunkTask(config->world.loadChunkBatch);
            loadChunkAccumTime_ -= loadChunkInterval;
        }
    }

    const float unloadChunkInterval = config->world.unloadChunkInterval;
    if (unloadChunkInterval == 0.0F) {
        ExecuteUnloadChunkTask(config->world.unloadChunkBatch);
    } else {
        unloadChunkAccumTime_ += delta;
        if (unloadChunkAccumTime_ > unloadChunkInterval) {
            ExecuteUnloadChunkTask(config->world.unloadChunkBatch);
            unloadChunkAccumTime_ -= unloadChunkInterval;
        }
    }

    const float unloadTerrainInterval = config->world.unloadTerrainInterval;
    if (unloadTerrainInterval == 0.0F) {
        ExecuteUnloadTerrainTask(config->world.unloadTerrainBatch);
    } else {
        unloadTerrainAccumTime_ += delta;
        if (unloadTerrainAccumTime_ > unloadTerrainInterval) {
            ExecuteUnloadTerrainTask(config->world.unloadTerrainBatch);
            unloadTerrainAccumTime_ -= unloadTerrainInterval;
        }
    }

    const float interval = config->world.chunkSpawnCleanInterval;
    if (interval == 0.0F) {
        // Execute for each frame, reset the timer, and directly skip the subsequent judgment.
        // 每帧都执行，重置计时器，直接跳过后续判断
        accumTime_ = 0.0F;
        firstTime_ = false;
    } else {
        accumTime_ += delta;
        // Intervals greater than 0 are executed according to normal timing.
        // 大于0的间隔，正常计时执行
        if (!firstTime_ && accumTime_ < interval) {
            return;
        }
        firstTime_ = false;
        accumTime_ -= interval;
    }
    WorldVector2D cameraPosition = cameraTransform2D->GetPosition();
    if (cameraPosition_.x == cameraPosition.x && cameraPosition_.y == cameraPosition.y) {
        return;
    }
    cameraPosition_.x = cameraPosition.x;
    cameraPosition_.y = cameraPosition.y;
    //Pre-calculated terrain range.
    //预计算的地形范围。
    auto preloadedTerrainViewportRect = viewportRect;
    const float preloadStructureRadius = config->world.preloadStructureRadius;
    preloadedTerrainViewportRect.x -= preloadStructureRadius * chunkWorldSize;
    preloadedTerrainViewportRect.y -= preloadStructureRadius * chunkWorldSize;
    preloadedTerrainViewportRect.w += preloadStructureRadius * 2 * chunkWorldSize;
    preloadedTerrainViewportRect.h += preloadStructureRadius * 2 * chunkWorldSize;
    //The complete range of pre-calculated blocks.
    //预计算的完整区块范围。
    auto preloadedChunkViewportRect = viewportRect;
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
            TileVector2D chunkVertexCoordinates(cx, cy);
            if (!WorldContext::ChunkIsOutOfBounds(chunkVertexCoordinates)) {
                auto chunkTaskPtr = std::make_unique<ChunkTask>(LoadTerrain, chunkVertexCoordinates);
                uint64_t signature = chunkTaskPtr->GetFingerprint();
                if (!taskFingerprintSet_.contains(signature)) {
                    PushTask(loadTerrainTasks_, std::move(chunkTaskPtr), signature);
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
            TileVector2D chunkVertexCoordinates(cx, cy);
            if (!WorldContext::ChunkIsOutOfBounds(chunkVertexCoordinates) && !worldContext_->HasChunk(
                    chunkVertexCoordinates)) {
                auto chunkTaskPtr = std::make_unique<ChunkTask>(LoadChunk, chunkVertexCoordinates);
                uint64_t signature = chunkTaskPtr->GetFingerprint();
                if (!taskFingerprintSet_.contains(signature)) {
                    PushTask(loadChunkTasks_, std::move(chunkTaskPtr), signature);
                }
            }
        }
    }

    // 卸载不可见区块
    std::unordered_map<TileVector2D, Chunk *, Vector2DIHash> &allChunks = *worldContext_->
            GetAllChunks();
    for (const
         auto &chunkVertexCoordinates: allChunks | std::views::keys) {
        if (chunkVertexCoordinates.x < startChunk.x || chunkVertexCoordinates.x >
            endChunk.x ||
            chunkVertexCoordinates.y < startChunk.y || chunkVertexCoordinates.y >
            endChunk.y
        ) {
            auto chunkTaskPtr = std::make_unique<ChunkTask>(UnloadChunk, chunkVertexCoordinates);
            uint64_t signature = chunkTaskPtr->GetFingerprint();
            if (!taskFingerprintSet_.contains(signature)) {
                PushTask(unloadChunkTasks_, std::move(chunkTaskPtr), signature);
            }
        }
    }

    std::unordered_map<TileVector2D, TerrainResult *, Vector2DIHash> &terrain = *worldContext_->GetTerrainResults();
    for (const
         auto &chunkVertexCoordinates: terrain | std::views::keys) {
        if (chunkVertexCoordinates.x < startTerrain.x || chunkVertexCoordinates.x >
            endTerrain.x ||
            chunkVertexCoordinates.y < startTerrain.y || chunkVertexCoordinates.y >
            endTerrain.y
        ) {
            auto chunkTaskPtr = std::make_unique<ChunkTask>(UnloadTerrain, chunkVertexCoordinates);
            uint64_t signature = chunkTaskPtr->GetFingerprint();
            if (!taskFingerprintSet_.contains(signature)) {
                PushTask(unloadTerrainTasks_, std::move(chunkTaskPtr), signature);
            }
        }
    }
    const TileVector2D originPosition = TileLayerComponent::WorldToTile(cameraPosition);
    //It needs to be saved in the set in reverse order.
    //需要反向保存在集合内。
    SetOriginAndSort(loadTerrainTasks_, originPosition, false);
    SetOriginAndSort(loadChunkTasks_, originPosition, false);
    SetOriginAndSort(unloadChunkTasks_, originPosition, true);
    SetOriginAndSort(unloadTerrainTasks_, originPosition, true);
}

std::string glimmer::ChunkSystem::GetName() {
    return "glimmer.ChunkSystem";
}
