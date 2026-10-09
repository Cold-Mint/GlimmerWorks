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
#include "core/world/Dimension.h"
#include "core/world/WorldContext.h"


std::shared_ptr<glimmer::TerrainResult> glimmer::TerrainGenerator::GenerateTerrain(const WorldContext *worldContext,
    const TerrainVertexVector2D &position) const {
    if (climateSampler_ == nullptr) {
        return nullptr;
    }
    const int worldSeed = worldContext->GetWorldSeed();
    const Dimension *dimension = worldContext->GetDimension();
    if (dimension == nullptr) {
        return nullptr;
    }
    const DimensionResource *dimensionResource = dimension->GetDimensionResource();
    if (dimensionResource == nullptr) {
        return nullptr;
    }
    const ResourceRef &dimensionRef = dimension->GetDimensionResourceRef();
    if (!dimensionRef.IsValid()) {
        return nullptr;
    }
    const AppContext *appContext = worldContext->GetAppContext();
    if (appContext == nullptr) {
        return nullptr;
    }
    const ModContext *modContext = appContext->GetModContext();
    if (modContext == nullptr) {
        return nullptr;
    }
    const BiomeRegistry *biomeRegistry = modContext->GetBiomeRegistry();
    if (biomeRegistry == nullptr) {
        return nullptr;
    }
    LogCat::d(LogLabel::TERRAIN, "terrain_generating", "Generating terrain: position=({}, {})", position.x, position.y);


    auto terrainResult = std::make_shared<TerrainResult>();
    terrainResult->SetPosition(position);
    //Compute base terrain data
    //计算基础地形数据。
    for (int localX = 0; localX < TERRAIN_SIZE; ++localX) {
        const int firstTileTerrainY = climateSampler_->GetFirstTileTerrainY(worldSeed, dimensionResource,
                                                                            position.x + localX);
        for (int localY = 0; localY < TERRAIN_SIZE; ++localY) {
            auto localPosition = TerrainRelativeVector2D(localX, localY);
            ComputeBaseTerrainTile(biomeRegistry, worldSeed, dimensionResource, dimensionRef,
                                   CoordinateTransformer::TerrainRelativeToTile(position, localPosition),
                                   firstTileTerrainY, terrainResult->GetMutableTerrainTileResult(localPosition));
        }
    }
    //Placement structure
    //放置结构
    StructurePlacementConditionsRegistry *structurePlacementConditionsRegistry = modContext->
            GetStructurePlacementConditionsRegistry();
    if (structurePlacementConditionsRegistry == nullptr) {
        return nullptr;
    }

    ResourceLocator *resourceLocator = appContext->GetResourceLocator();
    if (resourceLocator == nullptr) {
        return nullptr;
    }

    StructureRegistry *structureRegistry = modContext->GetStructureRegistry();
    if (structureRegistry == nullptr) {
        return nullptr;
    }
    StructureGeneratorManager *structureGeneratorManager = modContext->GetStructureGeneratorManager();
    if (structureGeneratorManager == nullptr) {
        return nullptr;
    }
    StructurePlacementConditionsProcessorManager *structurePlacementConditionsProcessorManager = modContext->
            GetStructurePlacementConditionsProcessorManager();
    if (structurePlacementConditionsProcessorManager == nullptr) {
        return nullptr;
    }
    const std::vector<IStructureResource *> &structureList = structureRegistry->GetAll();
    if (structureList.empty()) {
        return nullptr;
    }
    //Save the placement range of all structures.
    //保存所有结构的放置范围。
    const auto totalBitset = std::make_unique<std::bitset<TERRAIN_AREA> >();
    //全部设置为1（表示全部可放置）
    totalBitset->set();
    //Save a set of the overall placement points for a structure.
    //保存一个结构的总放置点集合。
    auto structureBitset = std::make_unique<std::bitset<TERRAIN_AREA> >();
    auto conditionBitset = std::make_unique<std::bitset<TERRAIN_AREA> >();
    for (const auto &structure: structureList) {
        auto &conditionList = structure->condition;
        if (conditionList.empty()) {
            continue;
        }
        structureBitset->set();
        for (const auto &condition: conditionList) {
            const IStructurePlacementConditionsResource *structurePlacementConditionsResource = resourceLocator->
                    FindStructurePlacementConditions(&condition);
            if (structurePlacementConditionsResource == nullptr) {
                continue;
            }
            IStructureConditionProcessor *structureConditionProcessor = structurePlacementConditionsProcessorManager->
                    FindConditionProcessors(
                        static_cast<StructureConditionProcessorType>(structurePlacementConditionsResource->
                            processorId));
            if (structureConditionProcessor == nullptr) {
                continue;
            }
            //Get structure placement points for this condition.
            //得到此条件的结构放置点。
            conditionBitset->reset();
            structureConditionProcessor->Match(
                dimensionResource, terrainResult.get(), structurePlacementConditionsResource, conditionBitset.get());
            *structureBitset &= *conditionBitset;
        }
        *structureBitset &= *totalBitset;
        if (structureBitset->none()) {
            continue;
        }
        //执行结构放置
        for (int i = 0; i < TERRAIN_AREA; ++i) {
            if (structureBitset->test(i)) {
                const int localX = i % TERRAIN_SIZE;
                const int localY = i / TERRAIN_SIZE;
                const TerrainRelativeVector2D relative(localX, localY);
                const TileVector2D structuralOrigin =
                        CoordinateTransformer::TerrainRelativeToTile(position, relative);
                std::unique_ptr<StructureInfo> structureInfo = structureGeneratorManager->Generate(
                    worldContext, structuralOrigin, structure);
                // 根据结构的最大和最小顶点，计算对应的地形顶点是否和现在生成的是同一个。(预先推算范围)
                TerrainVertexVector2D maxTerrainVertex = CoordinateTransformer::TileToTerrainVertex(
                    structureInfo->GetMaxPosition());
                if (maxTerrainVertex != position) {
                    //MaxPosition跨越了地形块
                    continue;
                }
                TerrainVertexVector2D minTerrainVertex = CoordinateTransformer::TileToTerrainVertex(
                    structureInfo->GetMinPosition());
                if (minTerrainVertex != position) {
                    //MinPosition跨越了地形块
                    continue;
                }
                const std::unordered_map<TileLayerType, std::unordered_map<TileVector2D, ResourceRef, Vector2DIHash> > &
                        structureMap = structureInfo->GetStructureMap();
                for (auto &[tileLayerType,tileMap]: structureMap) {
//TODO：在这里将结构放置到地形内Push进去，同时检查放置点，是否被占用（totalBitset->test(xxx xxx是当前瓦片的位置转地形相对位置) == 1）。
                }
            }
        }
    }


    LogCat::d(LogLabel::TERRAIN, "terrain_generation_completed", "Terrain generation completed: position=({}, {})",
              position.x,
              position.y);
    return terrainResult;
}

std::shared_ptr<glimmer::TerrainResult> glimmer::TerrainGenerator::GenerateOrGetTerrain(
    const WorldContext *worldContext,
    const TerrainVertexVector2D &position) {
    if (const auto iterator = terrainResults_.find(position); iterator != terrainResults_.end()) {
        if (auto terrainResult = iterator->second.lock()) {
            return terrainResult;
        }
        terrainResults_.erase(iterator);
    }
    auto terrainResult = GenerateTerrain(worldContext, position);
    if (terrainResult == nullptr) {
        return nullptr;
    }
    terrainResults_[position] = terrainResult;
    return terrainResult;
}

void glimmer::TerrainGenerator::ComputeBaseTerrainTile(const BiomeRegistry *biomeRegistry, int worldSeed,
                                                       const DimensionResource *dimensionResource,
                                                       const ResourceRef &dimension, const TileVector2D &world,
                                                       const int firstTileTerrainY,
                                                       TerrainTileResult &terrainTileResult) const {
    if (climateSampler_ == nullptr) {
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
        world.x < dimensionResource->minX) {
        terrainTileResult.SetTerrainType(TerrainResultType::VOID);
        return;
    }
    if (world.y == dimensionResource->minY) {
        terrainTileResult.SetTerrainType(TerrainResultType::BEDROCK);
    }
    if (world.y > firstTileTerrainY) {
        if (world.y < dimensionResource->seaLevelY) {
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
