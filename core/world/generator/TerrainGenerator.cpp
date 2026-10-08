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
#include "core/math/CoordinateTransformer.h"
#include "core/math/TerrainRelativeVector2D.h"
#include "core/mod/dataPack/BiomeRegistry.h"


std::shared_ptr<glimmer::TerrainResult> glimmer::TerrainGenerator::GenerateTerrain(const BiomeRegistry* biomeRegistry,
    int worldSeed, const DimensionResource* dimensionResource, const ResourceRef& dimension,
    const TerrainVertexVector2D& position) const
{
    if (climateSampler_ == nullptr)
    {
        return nullptr;
    }
    LogCat::d(LogLabel::TERRAIN, "terrain_generating", "Generating terrain: position=({}, {})", position.x, position.y);
    auto terrainResult = std::make_shared<TerrainResult>();
    terrainResult->SetPosition(position);
    for (int localX = 0; localX < TERRAIN_SIZE; ++localX)
    {
        const int firstTileTerrainY = climateSampler_->GetFirstTileTerrainY(worldSeed, dimensionResource,
                                                                            position.x + localX);
        for (int localY = 0; localY < TERRAIN_SIZE; ++localY)
        {
            auto localPosition = TerrainRelativeVector2D(localX, localY);
            WriteTerrainTileResult(biomeRegistry, worldSeed, dimensionResource, dimension,
                                   CoordinateTransformer::TerrainRelativeToTile(position, localPosition),
                                   firstTileTerrainY, terrainResult->GetMutableTerrainTileResult(localPosition));
        }
    }
    LogCat::d(LogLabel::TERRAIN, "terrain_generation_completed", "Terrain generation completed: position=({}, {})",
              position.x,
              position.y);
    return terrainResult;
}

std::shared_ptr<glimmer::TerrainResult> glimmer::TerrainGenerator::GenerateOrGetTerrain(
    const BiomeRegistry* biomeRegistry, const int worldSeed, const DimensionResource* dimensionResource,
    const ResourceRef& dimension, const TerrainVertexVector2D& position)
{
    if (const auto iterator = terrainResults_.find(position); iterator != terrainResults_.end())
    {
        if (auto terrainResult = iterator->second.lock())
        {
            return terrainResult;
        }
        terrainResults_.erase(iterator);
    }
    auto terrainResult = GenerateTerrain(biomeRegistry, worldSeed, dimensionResource, dimension, position);
    if (terrainResult == nullptr)
    {
        return nullptr;
    }
    terrainResults_[position] = terrainResult;
    return terrainResult;
}

void glimmer::TerrainGenerator::WriteTerrainTileResult(const BiomeRegistry* biomeRegistry, const int worldSeed,
                                                       const DimensionResource* dimensionResource,
                                                       const ResourceRef& dimension, const TileVector2D& world,
                                                       const int firstTileTerrainY,
                                                       TerrainTileResult& terrainTileResult) const
{
    if (climateSampler_ == nullptr)
    {
        return;
    }
    const float elevation = TerrainMath::GetElevation(dimensionResource, world.y);
    const auto humidity = climateSampler_->GetHumidity(worldSeed, dimensionResource, world);
    const auto temperature = climateSampler_->GetTemperature(worldSeed, dimensionResource, world, elevation);
    const auto weirdness = climateSampler_->GetWeirdness(worldSeed, dimensionResource, world);
    const auto erosion = climateSampler_->GetErosion(worldSeed, dimensionResource, world);
    const auto surfaceProximity = TerrainMath::GetSurfaceProximity(dimensionResource, firstTileTerrainY, world.y);
    terrainTileResult.SetWorldPosition(world);
    terrainTileResult.SetBiomeResource(biomeRegistry->FindBestBiome(dimension,
                                                                    humidity, temperature, weirdness, erosion,
                                                                    elevation,
                                                                    surfaceProximity));
    if (world.y > dimensionResource->maxY || world.y < dimensionResource->minY || world.x > dimensionResource->maxX ||
        world.x < dimensionResource->minX)
    {
        terrainTileResult.SetTerrainType(TerrainResultType::VOID);
        return;
    }
    if (world.y == dimensionResource->minY)
    {
        terrainTileResult.SetTerrainType(TerrainResultType::BEDROCK);
    }
    if (world.y > firstTileTerrainY)
    {
        if (world.y < dimensionResource->seaLevelY)
        {
            terrainTileResult.SetTerrainType(TerrainResultType::WATER);
            return;
        }
        terrainTileResult.SetTerrainType(TerrainResultType::AIR);
        return;
    }
    terrainTileResult.SetTerrainType(TerrainResultType::SOLID);
}

glimmer::ClimateSampler* glimmer::TerrainGenerator::GetMutableClimateSampler() const
{
    return climateSampler_.get();
}
