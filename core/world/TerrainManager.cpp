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
#include "TerrainManager.h"

#include <utility>

#include "core/config/Constants.h"
#include "core/log/LogCat.h"
#include "core/world/WorldContext.h"
#include "generator/ChunkGenerator.h"

glimmer::TerrainManager::TerrainManager(WorldContext *worldContext) : worldContext_(worldContext) {
}

const std::array<glimmer::TileVector2D, 8> &glimmer::TerrainManager::NeighborOffsets() {
    static const std::array offsets = {
        TileVector2D(0, CHUNK_SIZE), // up 上
        TileVector2D(0, -CHUNK_SIZE), // down 下
        TileVector2D(-CHUNK_SIZE, 0), // left 左
        TileVector2D(CHUNK_SIZE, 0), // right 右
        TileVector2D(-CHUNK_SIZE, CHUNK_SIZE), // up-left 左上
        TileVector2D(CHUNK_SIZE, CHUNK_SIZE), // up-right 右上
        TileVector2D(-CHUNK_SIZE, -CHUNK_SIZE), // down-left 左下
        TileVector2D(CHUNK_SIZE, -CHUNK_SIZE), // down-right 右下
    };
    return offsets;
}

void glimmer::TerrainManager::MarkReadyIfNeighborsGeneratedLocked(const TileVector2D &position) {
    if (!processedTerrainTiles_.contains(position)) {
        return;
    }
    for (const auto &offset: NeighborOffsets()) {
        const TileVector2D neighbor = position + offset;
        if (neighbor.y >= WORLD_MAX_Y || neighbor.y < WORLD_MIN_Y ||
            neighbor.x >= WORLD_MAX_X || neighbor.x < WORLD_MIN_X) {
            continue;
        }
        if (!processedTerrainTiles_.contains(neighbor)) {
            return;
        }
    }
    const auto it = terrainTileData_.find(position);
    if (it != terrainTileData_.end() && !it->second->IsReady()) {
        it->second->MakeReady();
        LogCat::d(LogLabel::TERRAIN, "terrain_marked_ready", "Terrain marked ready: position=({}, {})",
                  position.x, position.y);
    }
}

glimmer::TerrainResult *glimmer::TerrainManager::GetTerrainData(const TileVector2D &position) {
    std::lock_guard lock(mutex_);
    if (auto it = terrainTileData_.find(position); it != terrainTileData_.end()) {
        return it->second.get();
    }
    return nullptr;
}

glimmer::TerrainResult *glimmer::TerrainManager::GetOrCreateTerrainData(const TileVector2D &position) {
    {
        std::lock_guard lock(mutex_);
        if (auto it = terrainTileData_.find(position); it != terrainTileData_.end()) {
            return it->second.get();
        }
    }

    //The heavy terrain generation runs outside the lock.
    //重的地形生成在锁外执行。
    auto terrainResult = worldContext_->GetChunkGenerator()->GenerateTerrain(position);
    if (terrainResult == nullptr) {
        LogCat::w(LogLabel::TERRAIN, std::source_location::current(), "terrain_generate_failed",
                  "Failed to generate terrain data: position=({}, {})", position.x, position.y);
        return nullptr;
    }
    auto terrainPtr = terrainResult.get();
    {
        std::lock_guard lock(mutex_);
        if (auto it = terrainTileData_.find(position); it != terrainTileData_.end()) {
            //Another thread generated it first; discard ours.
            //其它线程先生成了，丢弃本次结果。
            return it->second.get();
        }
        terrainTileData_.emplace(position, std::move(terrainResult));
        terrainTileDataCache_.emplace(position, terrainPtr);
    }
    LogCat::d(LogLabel::TERRAIN, "terrain_data_created", "Created terrain data: position=({}, {})", position.x,
              position.y);
    return terrainPtr;
}

std::unordered_map<glimmer::TileVector2D, glimmer::TerrainResult *, glimmer::Vector2DIHash>
glimmer::TerrainManager::GetTerrainResults() {
    std::lock_guard lock(mutex_);
    return terrainTileDataCache_;
}

void glimmer::TerrainManager::LoadTerrainAt(TileVector2D position) {
    {
        std::lock_guard lock(mutex_);
        if (processedTerrainTiles_.contains(position)) {
            LogCat::e(LogLabel::TERRAIN, std::source_location::current(), "terrain_already_processed",
                      "Terrain is already processed: position=({}, {})", position.x, position.y);
            return;
        }
    }
    LogCat::d(LogLabel::TERRAIN, "terrain_loading", "Loading terrain (structure generation): position=({}, {})",
              position.x,
              position.y);
    //The heavy structure generation runs outside the lock.
    //重的结构生成在锁外执行。
    worldContext_->GetChunkGenerator()->GenerateStructure(position);
    {
        std::lock_guard lock(mutex_);
        processedTerrainTiles_.emplace(position);
        //The newly processed terrain may complete the readiness of itself or any
        //of its eight neighbors.
        //新完成结构生成的地形，可能使自己或八个邻居中的某一个达到就绪状态。
        MarkReadyIfNeighborsGeneratedLocked(position);
        for (const auto &offset: NeighborOffsets()) {
            MarkReadyIfNeighborsGeneratedLocked(position + offset);
        }
    }
}

void glimmer::TerrainManager::UnloadTerrainAt(TileVector2D position) {
    std::lock_guard lock(mutex_);
    if (!processedTerrainTiles_.contains(position)) {
        LogCat::e(LogLabel::TERRAIN, std::source_location::current(), "terrain_not_processed",
                  "Terrain is not processed: position=({}, {})", position.x, position.y);
        return;
    }
    LogCat::d(LogLabel::TERRAIN, "terrain_unloading", "Unloading terrain: position=({}, {})", position.x, position.y);
    processedTerrainTiles_.erase(position);
    terrainTileDataCache_.erase(position);
    terrainTileData_.erase(position);
}
