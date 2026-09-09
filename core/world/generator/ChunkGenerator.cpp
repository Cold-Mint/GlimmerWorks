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
#include "ChunkGenerator.h"

#include "BiomeDecoratorApplier.h"
#include "Chunk.h"
#include "ChunkTilePopulator.h"
#include "core/context/AppContext.h"
#include "core/context/ModContext.h"
#include "core/log/LogCat.h"
#include "core/mod/Resource.h"
#include "core/mod/ResourceLocator.h"
#include "BiomeDecoratorManager.h"
#include "core/mod/dataPack/BiomeRegistry.h"
#include "core/world/TerrainManager.h"
#include "core/world/WorldContext.h"

std::string glimmer::ChunkGenerator::ResolveDimensionId(const DimensionResource *dimensionResource) {
    const DimensionResource defaultDimension;
    if (dimensionResource == nullptr) {
        dimensionResource = &defaultDimension;
    }
    return Resource::GenerateId(dimensionResource->packId, dimensionResource->resourceId);
}

glimmer::BiomeRegistry *glimmer::ChunkGenerator::ResolveBiomeRegistry(WorldContext *worldContext) {
    if (worldContext == nullptr) {
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
    return modContext->GetBiomeRegistry();
}

glimmer::ChunkGenerator::ChunkGenerator(WorldContext *worldContext, const int worldSeed,
                                        const DimensionResource *dimensionResource)
    : worldContext_(worldContext),
      dimensionId_(ResolveDimensionId(dimensionResource)),
      terrainGenerator_(worldSeed, dimensionResource, dimensionId_, ResolveBiomeRegistry(worldContext)),
      structurePlacer_(worldContext),
      tileRefs_(TerrainTileRefs::Create()) {
}

std::unique_ptr<glimmer::TerrainResult> glimmer::ChunkGenerator::GenerateTerrain(const TileVector2D &position) {
    return terrainGenerator_.GenerateTerrain(position);
}

void glimmer::ChunkGenerator::GenerateStructure(const TileVector2D &position) const {
    structurePlacer_.GenerateStructure(position);
}

std::unique_ptr<glimmer::Chunk> glimmer::ChunkGenerator::GenerateChunkAt(const TileVector2D &position) const {
    if (worldContext_ == nullptr) {
        return nullptr;
    }
    AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr) {
        return nullptr;
    }
    ResourceLocator *resourceLocator = appContext->GetResourceLocator();
    if (resourceLocator == nullptr) {
        return nullptr;
    }
    BiomeDecoratorManager *biomeDecoratorManager = appContext->GetModContext()->GetBiomeDecoratorManager();
    if (biomeDecoratorManager == nullptr) {
        return nullptr;
    }
    Config *config = appContext->GetConfig();
    if (config == nullptr) {
        return nullptr;
    }
    LogCat::i("chunk_generating", "Generating new chunk at position: ({}, {})", position.x, position.y);
    auto chunk = std::make_unique<Chunk>(worldContext_, position, config);
    TerrainResult *terrainResult = worldContext_->GetTerrainManager()->GetTerrainData(position);
    if (terrainResult == nullptr) {
        LogCat::w(std::source_location::current(), "chunk_terrain_data_failed",
                  "Failed to get terrain data for chunk: ({}, {})", position.x,
                  position.y);
        return nullptr;
    }
    std::unordered_map<TileLayerType, std::array<ResourceRef, CHUNK_AREA> > tilesRefMap = {
        {TileLayerType::Ground, {}},
        {TileLayerType::BackGround, {}}
    };
    std::unordered_set<BiomeResource *> biomeResourcesSet;

    TileRefResolver::Initialize(terrainResult, tileRefs_, tilesRefMap, biomeResourcesSet);
    BiomeDecoratorApplier::Apply(biomeResourcesSet, resourceLocator, biomeDecoratorManager, worldContext_,
                                 terrainResult, tilesRefMap);
    ChunkTilePopulator::Populate(chunk.get(), resourceLocator, tilesRefMap);

    LogCat::i("chunk_generation_completed", "Chunk generation completed at: ({}, {})", position.x, position.y);
    return chunk;
}

int glimmer::ChunkGenerator::GetFirstTileTerrainY(const int x) {
    return terrainGenerator_.GetFirstTileTerrainY(x);
}

const std::string &glimmer::ChunkGenerator::GetDimensionId() const {
    return dimensionId_;
}

float glimmer::ChunkGenerator::GetHumidity(const TileVector2D &pos) {
    return terrainGenerator_.GetHumidity(pos);
}

float glimmer::ChunkGenerator::GetTemperature(const TileVector2D &pos, const float elevation) {
    return terrainGenerator_.GetTemperature(pos, elevation);
}

float glimmer::ChunkGenerator::GetWeirdness(const TileVector2D &pos) {
    return terrainGenerator_.GetWeirdness(pos);
}

float glimmer::ChunkGenerator::GetErosion(const TileVector2D &pos) {
    return terrainGenerator_.GetErosion(pos);
}
