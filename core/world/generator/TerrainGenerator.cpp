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
#include "core/mod/Resource.h"

glimmer::TerrainGenerator::TerrainGenerator(const int worldSeed, const DimensionResource *dimensionResource,
                                            std::string dimensionId, BiomeRegistry *biomeRegistry)
    : climateSampler_(worldSeed, dimensionResource),
      biomeMatcher_(std::move(dimensionId), biomeRegistry) {
}

std::unique_ptr<glimmer::TerrainResult> glimmer::TerrainGenerator::GenerateTerrain(const TileVector2D &position) {
    auto terrainResult = std::make_unique<TerrainResult>();
    terrainResult->SetPosition(position);
    for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
        const int firstTileTerrainY = GetFirstTileTerrainY(position.x + localX);
        for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
            terrainResult->SetTerrainTileResult(localX, localY,
                                                GetTerrainTileResult(position + TileVector2D(localX, localY),
                                                                     firstTileTerrainY));
        }
    }
    const int leftWorldX = position.x - 1;

    for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
        const int worldY = position.y + localY;
        const int firstTileTerrainY = GetFirstTileTerrainY(leftWorldX);

        terrainResult->SetLeftTerrainTileResult(
            localY,
            GetTerrainTileResult({leftWorldX, worldY}, firstTileTerrainY)
        );
    }

    const int rightWorldX = position.x + CHUNK_SIZE;

    for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
        const int worldY = position.y + localY;
        const int firstTileTerrainY = GetFirstTileTerrainY(rightWorldX);

        terrainResult->SetRightTerrainTileResult(
            localY,
            GetTerrainTileResult({rightWorldX, worldY}, firstTileTerrainY)
        );
    }

    const int downWorldY = position.y - 1;

    for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
        const int worldX = position.x + localX;
        const int firstTileTerrainY = GetFirstTileTerrainY(worldX);

        terrainResult->SetDownTerrainTileResult(
            localX,
            GetTerrainTileResult({worldX, downWorldY}, firstTileTerrainY)
        );
    }

    const int upWorldY = position.y + CHUNK_SIZE;

    for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
        const int worldX = position.x + localX;
        const int firstTileTerrainY = GetFirstTileTerrainY(worldX);

        terrainResult->SetUpTerrainTileResult(
            localX,
            GetTerrainTileResult({worldX, upWorldY}, firstTileTerrainY)
        );
    }

    return terrainResult;
}

TerrainTileResult glimmer::TerrainGenerator::GetTerrainTileResult(const TileVector2D &world,
                                                                  const int firstTileTerrainY) {
    TerrainTileResult terrainTileResult;
    const float elevation = TerrainMath::GetElevation(world.y);
    const auto humidity = GetHumidity(world);
    const auto temperature = GetTemperature(world, elevation);
    const auto weirdness = GetWeirdness(world);
    const auto erosion = GetErosion(world);
    const auto surfaceProximity = TerrainMath::GetSurfaceProximity(firstTileTerrainY, world.y);
    terrainTileResult.biomeResource = biomeMatcher_.Resolve(
        humidity, temperature, weirdness, erosion, elevation, surfaceProximity);
    if (world.y <= WORLD_MIN_Y || world.x == WORLD_MAX_X || world.x == WORLD_MIN_X) {
        terrainTileResult.terrainType = TerrainResultType::BEDROCK;
        return terrainTileResult;
    }
    if (world.y > firstTileTerrainY) {
        if (world.y < SEA_LEVEL_HEIGHT) {
            terrainTileResult.terrainType = TerrainResultType::WATER;
            return terrainTileResult;
        }
        terrainTileResult.terrainType = TerrainResultType::AIR;
        return terrainTileResult;
    }
    terrainTileResult.world = world;
    terrainTileResult.terrainType = TerrainResultType::SOLID;
    return terrainTileResult;
}

int glimmer::TerrainGenerator::GetFirstTileTerrainY(const int x) {
    return climateSampler_.GetFirstTileTerrainY(x);
}

float glimmer::TerrainGenerator::GetHumidity(const TileVector2D &pos) {
    return climateSampler_.GetHumidity(pos);
}

float glimmer::TerrainGenerator::GetTemperature(const TileVector2D &pos, const float elevation) {
    return climateSampler_.GetTemperature(pos, elevation);
}

float glimmer::TerrainGenerator::GetWeirdness(const TileVector2D &pos) {
    return climateSampler_.GetWeirdness(pos);
}

float glimmer::TerrainGenerator::GetErosion(const TileVector2D &pos) {
    return climateSampler_.GetErosion(pos);
}
