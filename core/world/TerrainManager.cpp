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

#include "core/log/LogCat.h"
#include "core/world/WorldContext.h"
#include "generator/ChunkGenerator.h"

glimmer::TerrainManager::TerrainManager(WorldContext *worldContext) : worldContext_(worldContext) {
}

glimmer::TerrainResult *glimmer::TerrainManager::GetTerrainData(const TileVector2D &position) {
    if (auto it = terrainTileData_.find(position); it != terrainTileData_.end()) {
        return it->second.get();
    }
    return nullptr;
}

glimmer::TerrainResult *glimmer::TerrainManager::GetOrCreateTerrainData(const TileVector2D &position) {
    if (auto it = terrainTileData_.find(position); it != terrainTileData_.end()) {
        return it->second.get();
    }

    auto terrainResult = worldContext_->GetChunkGenerator()->GenerateTerrain(position);
    if (terrainResult == nullptr) {
        LogCat::w(std::source_location::current(), "terrain_generate_failed",
                  "Failed to generate terrain data: position=({}, {})", position.x, position.y);
        return nullptr;
    }
    auto terrainPtr = terrainResult.get();
    terrainTileData_.emplace(position, std::move(terrainResult));
    terrainTileDataCache_.emplace(position, terrainPtr);
    LogCat::d("terrain_data_created", "Created terrain data: position=({}, {})", position.x, position.y);
    return terrainPtr;
}

std::unordered_map<glimmer::TileVector2D, glimmer::TerrainResult *, glimmer::Vector2DIHash> *
glimmer::TerrainManager::GetTerrainResults() {
    return &terrainTileDataCache_;
}

void glimmer::TerrainManager::LoadTerrainAt(TileVector2D position) {
    if (processedTerrainTiles_.contains(position)) {
        return;
    }
    LogCat::d("terrain_loading", "Loading terrain (structure generation): position=({}, {})", position.x,
              position.y);
    worldContext_->GetChunkGenerator()->GenerateStructure(position);
    processedTerrainTiles_.emplace(position);
}

void glimmer::TerrainManager::UnloadTerrainAt(TileVector2D position) {
    if (!processedTerrainTiles_.contains(position)) {
        return;
    }
    LogCat::d("terrain_unloading", "Unloading terrain: position=({}, {})", position.x, position.y);
    processedTerrainTiles_.erase(position);
    terrainTileDataCache_.erase(position);
    terrainTileData_.erase(position);
}
