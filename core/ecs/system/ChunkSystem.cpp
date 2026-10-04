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

#include <ranges>
#include <unordered_map>

#include "core/config/Config.h"
#include "core/config/Constants.h"
#include "core/math/CoordinateTransformer.h"
#include "core/math/Vector2DIHash.h"
#include "core/task/TaskWorker.h"
#include "core/world/ChunkManager.h"
#include "core/world/WorldContext.h"
#include "core/world/generator/Chunk.h"
#include "core/world/scheduler/ChunkTask.h"
#include "core/world/scheduler/ChunkTaskScheduler.h"
#include "core/world/scheduler/ChunkTaskType.h"

void glimmer::ChunkSystem::GenerateLoadTasks(const ResourceRef &dimensionRef, const ChunkManager *chunkManager,
                                             ChunkTaskScheduler *scheduler, const TileVector2D &startChunk,
                                             const TileVector2D &endChunk) {
    if (chunkManager == nullptr || scheduler == nullptr) {
        return;
    }
    for (int cy = startChunk.y; cy <= endChunk.y; cy += CHUNK_SIZE) {
        for (int cx = startChunk.x; cx <= endChunk.x; cx += CHUNK_SIZE) {
            const TileVector2D chunkVertexCoordinates(cx, cy);
            if (ChunkManager::ChunkIsOutOfBounds(chunkVertexCoordinates)) {
                continue;
            }
            if (chunkManager->HasChunk(dimensionRef, chunkVertexCoordinates)) {
                continue;
            }
            auto chunkTask = std::make_unique<ChunkTask>();
            chunkTask->SetDimensionResourceRef(dimensionRef);
            chunkTask->SetPosition(chunkVertexCoordinates);
            chunkTask->SetTaskType(ChunkTaskType::LOAD);
            scheduler->PushPendingTask(std::move(chunkTask));
        }
    }
}

void glimmer::ChunkSystem::GenerateUnloadTasks(const ResourceRef &dimensionRef, const ChunkManager *chunkManager,
                                               ChunkTaskScheduler *scheduler, const TileVector2D &startChunk,
                                               const TileVector2D &endChunk) {
    if (chunkManager == nullptr || scheduler == nullptr) {
        return;
    }
    const std::unordered_map<TileVector2D, std::unique_ptr<Chunk>, Vector2DIHash> *loadedChunks = chunkManager->
            GetLoadedChunks(dimensionRef);
    if (loadedChunks == nullptr) {
        return;
    }
    for (const auto &
         chunkVertexCoordinates: *loadedChunks | std::views::keys) {
        if (chunkVertexCoordinates.x >= startChunk.x && chunkVertexCoordinates.x <= endChunk.x &&
            chunkVertexCoordinates.y >= startChunk.y && chunkVertexCoordinates.y <= endChunk.y) {
            continue;
        }
        auto chunkTask = std::make_unique<ChunkTask>();
        chunkTask->SetDimensionResourceRef(dimensionRef);
        chunkTask->SetPosition(chunkVertexCoordinates);
        chunkTask->SetTaskType(ChunkTaskType::UNLOAD);
        scheduler->PushPendingTask(std::move(chunkTask));
    }
}

void glimmer::ChunkSystem::PostTask() {
    const WorldContext *worldContext = GetWorldContext();
    if (worldContext == nullptr) {
        return;
    }
    const AppContext *appContext = worldContext->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    const ModContext *modContext = appContext->GetModContext();
    if (modContext == nullptr) {
        return;
    }
    TaskWorker *taskWorker = appContext->GetTaskWorker();
    ChunkManager *chunkManager = worldContext->GetChunkManager();
    ChunkTaskScheduler *chunkTaskScheduler = worldContext->GetChunkTaskScheduler();
    StructureGeneratorManager *structureGeneratorManager = modContext->GetStructureGeneratorManager();
    if (taskWorker == nullptr || chunkManager == nullptr || chunkTaskScheduler == nullptr || structureGeneratorManager
        == nullptr) {
        return;
    }
    chunkTaskInProgress_.store(true);
    taskWorker->PostTask([chunkManager, chunkTaskScheduler, this, structureGeneratorManager] {
        while (const std::unique_ptr<ChunkTask> chunkTask = chunkTaskScheduler->PopFrontTask()) {
            const uint32_t maxChunksOccupiedByStructure = structureGeneratorManager->GetMaxChunksOccupiedByStructure();
            switch (chunkTask->GetTaskType()) {
                case ChunkTaskType::LOAD:
                    chunkManager->LoadChunkAt(maxChunksOccupiedByStructure, chunkTask->GetDimensionResourceRef(),
                                              chunkTask->GetPosition());
                    break;
                case ChunkTaskType::UNLOAD:
                    chunkManager->UnloadChunkAt(chunkTask->GetDimensionResourceRef(), chunkTask->GetPosition());
                    break;
                case ChunkTaskType::CANCELLED:
                default:
                    break;
            }
        }
        chunkTaskInProgress_.store(false);
    });
}

glimmer::ChunkSystem::ChunkSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    WatchComponent(COMPONENT_CAMERA);
    WatchComponent(COMPONENT_TRANSFORM_2D);
    Init();
}

void glimmer::ChunkSystem::OnWatchedComponentChanged(const GameComponentTypeMessage gameComponentType, uint32_t count) {
    const EntityShortCut *entityShortCut = GetEntityShortCut();
    if (entityShortCut == nullptr) {
        return;
    }
    if (gameComponentType == COMPONENT_CAMERA) {
        cameraComponent_ = entityShortCut->GetCameraComponent();
    }
    if (gameComponentType == COMPONENT_TRANSFORM_2D) {
        cameraTransform2DComponent_ = entityShortCut->GetCameraTransform2DComponent();
    }
}

void glimmer::ChunkSystem::OnTick(const uint64_t tick) {
    const WorldContext *worldContext = GetWorldContext();
    if (worldContext == nullptr || cameraComponent_ == nullptr || cameraTransform2DComponent_ == nullptr) {
        return;
    }
    const Dimension *dimension = worldContext->GetDimension();
    if (dimension == nullptr) {
        return;
    }
    const ResourceRef &dimensionResourceRef = dimension->GetDimensionResourceRef();
    if (!dimensionResourceRef.IsValid()) {
        return;
    }
    float preloadChunkRadius;
    uint8_t chunkScanTaskTickInterval;
    {
        std::lock_guard lock(worldConfigMutex_);
        preloadChunkRadius = preloadChunkRadius_;
        chunkScanTaskTickInterval = chunkScanTaskTickInterval_;
    }

    //Has it reached the interval for configuration?
    //是否到了配置的间隔。
    if (tick % chunkScanTaskTickInterval != 0) {
        return;
    }

    //Check if the camera and its position have changed.
    //检测相机，位置是否改变。
    bool cameraChanged = false;
    if (const uint32_t cameraVersion = cameraComponent_->GetVersion(); cameraLastVersion_ != cameraVersion) {
        cameraLastVersion_ = cameraVersion;
        cameraChanged = true;
    }
    if (const uint32_t cameraTransformLastVersion = cameraTransform2DComponent_->GetVersion();
        cameraTransformLastVersion_ == cameraTransformLastVersion) {
        cameraTransformLastVersion_ = cameraTransformLastVersion;
        cameraChanged = true;
    }
    if (!cameraChanged) {
        return;
    }

    const auto viewportRect = CoordinateTransformer::GetViewportRect(cameraTransform2DComponent_->GetPosition(),
                                                                     cameraComponent_->GetSize(),
                                                                     cameraComponent_->GetZoom());
    auto preloadedChunkViewportRect = viewportRect;
    preloadedChunkViewportRect.x -= preloadChunkRadius * CHUNK_WORLD_SIZE;
    preloadedChunkViewportRect.y -= preloadChunkRadius * CHUNK_WORLD_SIZE;
    preloadedChunkViewportRect.w += preloadChunkRadius * 2 * CHUNK_WORLD_SIZE;
    preloadedChunkViewportRect.h += preloadChunkRadius * 2 * CHUNK_WORLD_SIZE;

    const TileVector2D topLeftChunkCorner = CoordinateTransformer::WorldToTile(
        WorldVector2D(preloadedChunkViewportRect.x, preloadedChunkViewportRect.y));
    const TileVector2D lowerRightChunkCorner = CoordinateTransformer::WorldToTile(
        WorldVector2D(preloadedChunkViewportRect.x + preloadedChunkViewportRect.w,
                      preloadedChunkViewportRect.y + preloadedChunkViewportRect.h));
    const TileVector2D startChunk = Chunk::TileCoordinatesToChunkVertexCoordinates(topLeftChunkCorner);
    const TileVector2D endChunk = Chunk::TileCoordinatesToChunkVertexCoordinates(lowerRightChunkCorner);

    ChunkTaskScheduler *chunkTaskScheduler = worldContext->GetChunkTaskScheduler();
    if (chunkTaskScheduler == nullptr) {
        return;
    }
    ChunkManager *chunkManager = worldContext->GetChunkManager();
    if (chunkManager == nullptr) {
        return;
    }

    GenerateLoadTasks(dimensionResourceRef, chunkManager, chunkTaskScheduler, startChunk, endChunk);
    GenerateUnloadTasks(dimensionResourceRef, chunkManager, chunkTaskScheduler, startChunk, endChunk);

    chunkTaskScheduler->Commit();
    const TileVector2D originPosition = CoordinateTransformer::WorldToTile(cameraTransform2DComponent_->GetPosition());
    chunkTaskScheduler->SortTask(originPosition);

    if (chunkTaskScheduler->GetMainTaskCount() > 0 && !chunkTaskInProgress_.load()) {
        //If it is discovered that there are tasks that have not been delivered to the worker thread and have not been delivered before, then the delivery will be executed.
        //发现有尚未投递到工作线程的任务，且没有投递过。那么执行投递。
        PostTask();
    }
}

void glimmer::ChunkSystem::OnConfigChanged(const Config *config) {
    if (config == nullptr) {
        return;
    }
    std::lock_guard lock(worldConfigMutex_);
    chunkScanTaskTickInterval_ = config->world.chunkScanTaskTickInterval;
    preloadChunkRadius_ = config->world.preloadChunkRadius;
}

glimmer::GameSystemType glimmer::ChunkSystem::GetGameSystemType() const {
    return GameSystemType::ChunkSystem;
}
