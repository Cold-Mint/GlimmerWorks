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
#include "TerrainGenerator.h"

#include <utility>

#include "TerrainResultType.h"
#include "TerrainMath.h"
#include "core/config/Constants.h"
#include "core/log/LogCat.h"
#include "core/mod/dataPack/BiomeRegistry.h"


std::shared_ptr<glimmer::TerrainResult> glimmer::TerrainGenerator::GenerateTerrain(const BiomeRegistry *biomeRegistry,
    const ResourceRef &dimension, const TileVector2D &position) const {
    if (climateSampler_ == nullptr) {
        return nullptr;
    }
    LogCat::d(LogLabel::TERRAIN, "terrain_generating", "Generating terrain: position=({}, {})", position.x, position.y);
    auto terrainResult = std::make_shared<TerrainResult>();
    terrainResult->SetPosition(position);
    for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
        const int firstTileTerrainY = climateSampler_->GetFirstTileTerrainY(position.x + localX);
        for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
            auto localPosition = TileVector2D(localX, localY);
            WriteTerrainTileResult(biomeRegistry, dimension, localPosition + position, firstTileTerrainY,
                                   terrainResult->GetMutableTerrainTileResult(localPosition));
        }
    }
    const int upWorldY = position.y + CHUNK_SIZE;
    for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
        const int worldX = position.x + localX;
        const int firstTileTerrainY = climateSampler_->GetFirstTileTerrainY(worldX);
        auto worldPosition = TileVector2D(position.x + localX, upWorldY);
        WriteTerrainTileResult(biomeRegistry, dimension, worldPosition, firstTileTerrainY,
                               terrainResult->GetMutableUpTerrainTileResult(localX));
    }
    LogCat::d(LogLabel::TERRAIN, "terrain_generation_completed", "Terrain generation completed: position=({}, {})",
              position.x,
              position.y);
    return terrainResult;
}

std::shared_ptr<glimmer::TerrainResult> glimmer::TerrainGenerator::GenerateOrGetTerrain(
    const BiomeRegistry *biomeRegistry, const ResourceRef &dimension, const TileVector2D &position) {
    if (const auto iterator = terrainResults_.find(position); iterator != terrainResults_.end()) {
        if (auto terrainResult = iterator->second.lock()) {
            return terrainResult;
        }
        terrainResults_.erase(iterator);
    }
    auto terrainResult = GenerateTerrain(biomeRegistry, dimension, position);
    if (terrainResult == nullptr) {
        return nullptr;
    }
    terrainResults_[position] = terrainResult;
    return terrainResult;
}

void glimmer::TerrainGenerator::WriteTerrainTileResult(const BiomeRegistry *biomeRegistry, const ResourceRef &dimension,
                                                       const TileVector2D &world, const int firstTileTerrainY,
                                                       TerrainTileResult &terrainTileResult) const {
    if (climateSampler_ == nullptr) {
        return;
    }
    const float elevation = TerrainMath::GetElevation(world.y);
    const auto humidity = climateSampler_->GetHumidity(world);
    const auto temperature = climateSampler_->GetTemperature(world, elevation);
    const auto weirdness = climateSampler_->GetWeirdness(world);
    const auto erosion = climateSampler_->GetErosion(world);
    const auto surfaceProximity = TerrainMath::GetSurfaceProximity(firstTileTerrainY, world.y);
    terrainTileResult.SetWorldPosition(world);
    terrainTileResult.SetBiomeResource(biomeRegistry->FindBestBiome(dimension,
                                                                    humidity, temperature, weirdness, erosion,
                                                                    elevation,
                                                                    surfaceProximity));
    if (world.y > WORLD_MAX_Y || world.y < WORLD_MIN_Y || world.x > WORLD_MAX_X || world.x < WORLD_MIN_X) {
        terrainTileResult.SetTerrainType(TerrainResultType::VOID);
        return;
    }
    if (world.y == WORLD_MIN_Y) {
        terrainTileResult.SetTerrainType(TerrainResultType::BEDROCK);
    }
    if (world.y > firstTileTerrainY) {
        if (world.y < SEA_LEVEL_HEIGHT) {
            terrainTileResult.SetTerrainType(TerrainResultType::WATER);
            return;
        }
        terrainTileResult.SetTerrainType(TerrainResultType::AIR);
        return;
    }
    terrainTileResult.SetTerrainType(TerrainResultType::SOLID);
}

glimmer::ClimateSampler *glimmer::TerrainGenerator::GetMutableClimateSampler() const {
    return climateSampler_.get();
}
