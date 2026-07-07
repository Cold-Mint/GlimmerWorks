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

#include "core/Constants.h"
#include <functional>
#include "core/world/WorldContext.h"
#include "core/world/ChunkManager.h"
#include "core/world/TerrainManager.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/math/CoordinateTransformer.h"
#include "core/world/generator/Chunk.h"


void glimmer::ChunkSystem::ExecuteLoadTerrainTask(const uint16_t loadTerrainBatch)
{
    WorldContext* worldContext = GetWorldContext();
    if (loadTerrainTasks_.empty())
    {
        return;
    }
    if (loadTerrainBatch == 0)
    {
        while (!loadTerrainTasks_.empty())
        {
            auto task = std::move(loadTerrainTasks_.back());
            taskFingerprintSet_.erase(task->GetFingerprint());
            worldContext->GetTerrainManager()->LoadTerrainAt(task->GetChunkVertexCoordinates());
            loadTerrainTasks_.pop_back();
        }
    }
    else
    {
        for (uint16_t i = 0; i < loadTerrainBatch; i++)
        {
            if (loadTerrainTasks_.empty())
            {
                break;
            }
            auto task = std::move(loadTerrainTasks_.back());
            taskFingerprintSet_.erase(task->GetFingerprint());
            worldContext->GetTerrainManager()->LoadTerrainAt(task->GetChunkVertexCoordinates());
            loadTerrainTasks_.pop_back();
        }
    }
}

void glimmer::ChunkSystem::ExecuteLoadChunkTask(const uint16_t loadChunkBatch)
{
    WorldContext* worldContext = GetWorldContext();
    if (loadChunkTasks_.empty())
    {
        return;
    }

    if (loadChunkBatch == 0)
    {
        while (!loadChunkTasks_.empty())
        {
            auto task = std::move(loadChunkTasks_.back());
            taskFingerprintSet_.erase(task->GetFingerprint());
            worldContext->GetChunkManager()->LoadChunkAt(task->GetChunkVertexCoordinates());
            loadChunkTasks_.pop_back();
        }
    }
    else
    {
        for (uint16_t i = 0; i < loadChunkBatch; i++)
        {
            if (loadChunkTasks_.empty())
            {
                break;
            }
            auto task = std::move(loadChunkTasks_.back());
            taskFingerprintSet_.erase(task->GetFingerprint());
            worldContext->GetChunkManager()->LoadChunkAt(task->GetChunkVertexCoordinates());
            loadChunkTasks_.pop_back();
        }
    }
}

void glimmer::ChunkSystem::ExecuteUnloadChunkTask(const uint16_t unloadChunkBatch)
{
    WorldContext* worldContext = GetWorldContext();
    if (unloadChunkTasks_.empty())
    {
        return;
    }
    if (unloadChunkBatch == 0)
    {
        while (!unloadChunkTasks_.empty())
        {
            auto task = std::move(unloadChunkTasks_.back());
            taskFingerprintSet_.erase(task->GetFingerprint());
            worldContext->GetChunkManager()->UnloadChunkAt(task->GetChunkVertexCoordinates());
            unloadChunkTasks_.pop_back();
        }
    }
    else
    {
        for (uint16_t i = 0; i < unloadChunkBatch; i++)
        {
            if (unloadChunkTasks_.empty())
            {
                break;
            }
            auto task = std::move(unloadChunkTasks_.back());
            taskFingerprintSet_.erase(task->GetFingerprint());
            worldContext->GetChunkManager()->UnloadChunkAt(task->GetChunkVertexCoordinates());
            unloadChunkTasks_.pop_back();
        }
    }
}

void glimmer::ChunkSystem::ExecuteUnloadTerrainTask(const uint16_t unloadTerrainBatch)
{
    WorldContext* worldContext = GetWorldContext();
    if (unloadTerrainTasks_.empty())
    {
        return;
    }
    if (unloadTerrainBatch == 0)
    {
        while (!unloadTerrainTasks_.empty())
        {
            auto task = std::move(unloadTerrainTasks_.back());
            taskFingerprintSet_.erase(task->GetFingerprint());
            worldContext->GetTerrainManager()->UnloadTerrainAt(task->GetChunkVertexCoordinates());
            unloadTerrainTasks_.pop_back();
        }
    }
    else
    {
        for (uint16_t i = 0; i < unloadTerrainBatch; i++)
        {
            if (unloadTerrainTasks_.empty())
            {
                break;
            }
            auto task = std::move(unloadTerrainTasks_.back());
            taskFingerprintSet_.erase(task->GetFingerprint());
            worldContext->GetTerrainManager()->UnloadTerrainAt(task->GetChunkVertexCoordinates());
            unloadTerrainTasks_.pop_back();
        }
    }
}

void glimmer::ChunkSystem::PushTask(std::vector<std::unique_ptr<ChunkTask>>& taskList,
                                    std::unique_ptr<ChunkTask> chunkTask, const uint64_t fingerprint)
{
    taskList.push_back(std::move(chunkTask));
    taskFingerprintSet_.insert(fingerprint);
}

void glimmer::ChunkSystem::SetOriginAndSort(std::vector<std::unique_ptr<ChunkTask>>& taskList, TileVector2D origin,
                                            bool sortAscending)
{
    for (const auto& task : taskList)
    {
        task->SetOrigin(origin);
    }
    std::sort(taskList.begin(), taskList.end(), [sortAscending](const auto& a, const auto& b)
    {
        const auto distA = a->GetDistance();
        const auto distB = b->GetDistance();
        return sortAscending ? distA < distB : distA > distB;
    });
}

void glimmer::ChunkSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t number)
{
    const EntityShortCut* entityShortCut = GetEntityShortCut();
    if (gameComponentType == COMPONENT_CAMERA)
    {
        cameraComponent_ = entityShortCut->GetCameraComponent();
    }
    if (gameComponentType == COMPONENT_TRANSFORM_2D)
    {
        cameraTransform2DComponent_ = entityShortCut->GetCameraTransform2DComponent();
    }
}

glimmer::ChunkSystem::ChunkSystem(WorldContext* worldContext)
    : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_CAMERA);
    WatchComponent(COMPONENT_TRANSFORM_2D);
    Init();
}

void glimmer::ChunkSystem::UpdateChunkFadeAnimation(float delta, const SDL_FRect& viewportRect) const
{
    const WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        return;
    }
    ChunkManager* chunkManager = worldContext->GetChunkManager();
    if (chunkManager == nullptr)
    {
        return;
    }
    constexpr float chunkWorldSize = CHUNK_SIZE * TILE_SIZE;
    const TileVector2D topLeft = CoordinateTransformer::WorldToTile({
        viewportRect.x - chunkWorldSize, viewportRect.y - chunkWorldSize
    });
    const TileVector2D bottomRight = CoordinateTransformer::WorldToTile({
        viewportRect.x + viewportRect.w + chunkWorldSize,
        viewportRect.y + viewportRect.h + chunkWorldSize
    });
    for (int x = topLeft.x; x < bottomRight.x; x += CHUNK_SIZE)
    {
        for (int y = topLeft.y; y < bottomRight.y; y += CHUNK_SIZE)
        {
            Chunk* chunk = chunkManager->GetChunk(
                Chunk::TileCoordinatesToChunkVertexCoordinates(TileVector2D(x, y)));
            if (chunk == nullptr)
            {
                continue;
            }
            chunk->UpdateFadeInAnimation(delta);
        }
    }
}



bool glimmer::ChunkSystem::UpdateCameraPosition()
{
    const WorldVector2D cameraPosition = cameraTransform2DComponent_->GetPosition();
    if (cameraPosition_.x == cameraPosition.x && cameraPosition_.y == cameraPosition.y)
    {
        return false;
    }
    cameraPosition_.x = cameraPosition.x;
    cameraPosition_.y = cameraPosition.y;
    return true;
}

void glimmer::ChunkSystem::GenerateLoadTerrainTasks(const TileVector2D& startTerrain, const TileVector2D& endTerrain)
{
    for (int cy = startTerrain.y; cy < endTerrain.y; cy += CHUNK_SIZE)
    {
        for (int cx = startTerrain.x; cx < endTerrain.x; cx += CHUNK_SIZE)
        {
            TileVector2D chunkVertexCoordinates(cx, cy);
            if (ChunkManager::ChunkIsOutOfBounds(chunkVertexCoordinates))
            {
                continue;
            }
            auto chunkTaskPtr = std::make_unique<ChunkTask>(ChunkType::LoadTerrain, chunkVertexCoordinates);
            uint64_t signature = chunkTaskPtr->GetFingerprint();
            if (taskFingerprintSet_.contains(signature))
            {
                continue;
            }
            PushTask(loadTerrainTasks_, std::move(chunkTaskPtr), signature);
        }
    }
}

void glimmer::ChunkSystem::GenerateLoadChunkTasks(const TileVector2D& startChunk, const TileVector2D& endChunk)
{
    const WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        return;
    }
    const ChunkManager* chunkManager = worldContext->GetChunkManager();
    if (chunkManager == nullptr)
    {
        return;
    }
    for (int cy = startChunk.y; cy <= endChunk.y; cy += CHUNK_SIZE)
    {
        for (int cx = startChunk.x; cx <= endChunk.x; cx += CHUNK_SIZE)
        {
            TileVector2D chunkVertexCoordinates(cx, cy);
            if (ChunkManager::ChunkIsOutOfBounds(chunkVertexCoordinates))
            {
                continue;
            }
            if (chunkManager->HasChunk(chunkVertexCoordinates))
            {
                continue;
            }
            auto chunkTaskPtr = std::make_unique<ChunkTask>(ChunkType::LoadChunk, chunkVertexCoordinates);
            uint64_t signature = chunkTaskPtr->GetFingerprint();
            if (taskFingerprintSet_.contains(signature))
            {
                continue;
            }
            PushTask(loadChunkTasks_, std::move(chunkTaskPtr), signature);
        }
    }
}

void glimmer::ChunkSystem::GenerateUnloadChunkTasks(const TileVector2D& startChunk, const TileVector2D& endChunk)
{
    WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        return;
    }
    ChunkManager* chunkManager = worldContext->GetChunkManager();
    if (chunkManager == nullptr)
    {
        return;
    }
    std::unordered_map<TileVector2D, Chunk*, Vector2DIHash>& allChunks = *chunkManager->
        GetAllChunks();
    for (const auto& chunkVertexCoordinates : allChunks | std::views::keys)
    {
        if (chunkVertexCoordinates.x >= startChunk.x && chunkVertexCoordinates.x <= endChunk.x &&
            chunkVertexCoordinates.y >= startChunk.y && chunkVertexCoordinates.y <= endChunk.y)
        {
            continue;
        }
        auto chunkTaskPtr = std::make_unique<ChunkTask>(ChunkType::UnloadChunk, chunkVertexCoordinates);
        uint64_t signature = chunkTaskPtr->GetFingerprint();
        if (taskFingerprintSet_.contains(signature))
        {
            continue;
        }
        PushTask(unloadChunkTasks_, std::move(chunkTaskPtr), signature);
    }
}

void glimmer::ChunkSystem::GenerateUnloadTerrainTasks(const TileVector2D& startTerrain, const TileVector2D& endTerrain)
{
    WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        return;
    }
     TerrainManager* terrainManager = worldContext->GetTerrainManager();
    if (terrainManager == nullptr)
    {
        return;
    }
    std::unordered_map<TileVector2D, TerrainResult*, Vector2DIHash>& terrain = *terrainManager->
        GetTerrainResults();
    for (const auto& chunkVertexCoordinates : terrain | std::views::keys)
    {
        if (chunkVertexCoordinates.x >= startTerrain.x && chunkVertexCoordinates.x <= endTerrain.x &&
            chunkVertexCoordinates.y >= startTerrain.y && chunkVertexCoordinates.y <= endTerrain.y)
        {
            continue;
        }
        auto chunkTaskPtr = std::make_unique<ChunkTask>(ChunkType::UnloadTerrain, chunkVertexCoordinates);
        uint64_t signature = chunkTaskPtr->GetFingerprint();
        if (taskFingerprintSet_.contains(signature))
        {
            continue;
        }
        PushTask(unloadTerrainTasks_, std::move(chunkTaskPtr), signature);
    }
}

void glimmer::ChunkSystem::Update(const float delta)
{
    WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        return;
    }
    if (cameraComponent_ == nullptr)
    {
        return;
    }
    if (cameraTransform2DComponent_ == nullptr)
    {
        return;
    }
    constexpr float chunkWorldSize = CHUNK_SIZE * TILE_SIZE;
    const auto viewportRect = CoordinateTransformer::GetViewportRect(cameraTransform2DComponent_->GetPosition(),
                                                                     cameraComponent_->GetSize(),
                                                                     cameraComponent_->GetZoom());
    UpdateChunkFadeAnimation(delta, viewportRect);

    const AppContext* appContext = worldContext->GetAppContext();
    if (appContext == nullptr)
    {
        return;
    }
    const Config* config = appContext->GetConfig();
    if (config == nullptr)
    {
        return;
    }
    ExecuteTimedTask(delta, config->world.loadTerrainInterval, loadTerrainAccumTime_,
                     config->world.loadTerrainBatch, [this](uint16_t batch) { ExecuteLoadTerrainTask(batch); });
    ExecuteTimedTask(delta, config->world.loadChunkInterval, loadChunkAccumTime_,
                     config->world.loadChunkBatch, [this](uint16_t batch) { ExecuteLoadChunkTask(batch); });
    ExecuteTimedTask(delta, config->world.unloadChunkInterval, unloadChunkAccumTime_,
                     config->world.unloadChunkBatch, [this](uint16_t batch) { ExecuteUnloadChunkTask(batch); });
    ExecuteTimedTask(delta, config->world.unloadTerrainInterval, unloadTerrainAccumTime_,
                     config->world.unloadTerrainBatch, [this](uint16_t batch) { ExecuteUnloadTerrainTask(batch); });

    const float interval = config->world.chunkSpawnCleanInterval;
    if (interval == 0.0F)
    {
        accumTime_ = 0.0F;
        firstTime_ = false;
    }
    else
    {
        accumTime_ += delta;
        if (!firstTime_ && accumTime_ < interval)
        {
            return;
        }
        firstTime_ = false;
        accumTime_ -= interval;
    }
    if (!UpdateCameraPosition())
    {
        return;
    }
    auto preloadedTerrainViewportRect = viewportRect;
    const float preloadStructureRadius = config->world.preloadStructureRadius;
    preloadedTerrainViewportRect.x -= preloadStructureRadius * chunkWorldSize;
    preloadedTerrainViewportRect.y -= preloadStructureRadius * chunkWorldSize;
    preloadedTerrainViewportRect.w += preloadStructureRadius * 2 * chunkWorldSize;
    preloadedTerrainViewportRect.h += preloadStructureRadius * 2 * chunkWorldSize;
    auto preloadedChunkViewportRect = viewportRect;
    const float preloadChunkRadius = config->world.preloadChunkRadius;
    preloadedChunkViewportRect.x -= preloadChunkRadius * chunkWorldSize;
    preloadedChunkViewportRect.y -= preloadChunkRadius * chunkWorldSize;
    preloadedChunkViewportRect.w += preloadChunkRadius * 2 * chunkWorldSize;
    preloadedChunkViewportRect.h += preloadChunkRadius * 2 * chunkWorldSize;

    TileVector2D topLeftTerrainCorner = CoordinateTransformer::WorldToTile(
        WorldVector2D(preloadedTerrainViewportRect.x, preloadedTerrainViewportRect.y));
    TileVector2D lowerRightTerrainCorner = CoordinateTransformer::WorldToTile(
        WorldVector2D(preloadedTerrainViewportRect.x + preloadedTerrainViewportRect.w,
                      preloadedTerrainViewportRect.y + preloadedTerrainViewportRect.h));
    const TileVector2D startTerrain = Chunk::TileCoordinatesToChunkVertexCoordinates(topLeftTerrainCorner);
    const TileVector2D endTerrain = Chunk::TileCoordinatesToChunkVertexCoordinates(lowerRightTerrainCorner);

    const TileVector2D topLeftChunkCorner = CoordinateTransformer::WorldToTile(
        WorldVector2D(preloadedChunkViewportRect.x, preloadedChunkViewportRect.y));
    const TileVector2D lowerRightChunkCorner = CoordinateTransformer::WorldToTile(
        WorldVector2D(preloadedChunkViewportRect.x + preloadedChunkViewportRect.w,
                      preloadedChunkViewportRect.y + preloadedChunkViewportRect.h));
    const TileVector2D startChunk = Chunk::TileCoordinatesToChunkVertexCoordinates(topLeftChunkCorner);
    const TileVector2D endChunk = Chunk::TileCoordinatesToChunkVertexCoordinates(lowerRightChunkCorner);

    GenerateLoadTerrainTasks(startTerrain, endTerrain);
    GenerateLoadChunkTasks(startChunk, endChunk);
    GenerateUnloadChunkTasks(startChunk, endChunk);
    GenerateUnloadTerrainTasks(startTerrain, endTerrain);

    const TileVector2D originPosition = CoordinateTransformer::WorldToTile(cameraPosition_);
    SetOriginAndSort(loadTerrainTasks_, originPosition, false);
    SetOriginAndSort(loadChunkTasks_, originPosition, false);
    SetOriginAndSort(unloadChunkTasks_, originPosition, true);
    SetOriginAndSort(unloadTerrainTasks_, originPosition, true);
}

glimmer::GameSystemType glimmer::ChunkSystem::GetGameSystemType() const
{
    return GameSystemType::ChunkSystem;
}
