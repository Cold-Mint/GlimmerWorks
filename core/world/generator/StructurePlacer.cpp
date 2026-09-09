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
#include "StructurePlacer.h"

#include <climits>
#include <cstddef>

#include "core/context/AppContext.h"
#include "core/context/ModContext.h"
#include "core/math/TileVector2D.h"
#include "core/mod/Resource.h"
#include "core/mod/ResourceRef.h"
#include "core/mod/StructurePlacementConditionsProcessorManager.h"
#include "core/mod/dataPack/StructurePlacementConditionsRegistry.h"
#include "core/mod/dataPack/StructureRegistry.h"
#include "core/world/TerrainManager.h"
#include "core/world/WorldContext.h"
#include "core/world/generator/TerrainResult.h"
#include "core/world/structure/IStructureConditionProcessor.h"
#include "core/world/structure/StructureConditionProcessorType.h"
#include "core/world/structure/StructureGeneratorManager.h"
#include "core/world/structure/StructureInfo.h"

glimmer::StructurePlacer::StructurePlacer(WorldContext *worldContext) : worldContext_(worldContext) {
}

void glimmer::StructurePlacer::GenerateStructure(const TileVector2D &position) const {
    const AppContext *appContext = worldContext_->GetAppContext();
    const auto &all = appContext->GetModContext()->GetStructureRegistry()->GetAll();
    if (all.empty()) {
        return;
    }

    TerrainManager *terrainManager = worldContext_->GetTerrainManager();
    if (terrainManager == nullptr) {
        return;
    }

    TerrainResult *terrainResult = terrainManager->GetOrCreateTerrainData(position);

    for (auto structureResource: all) {
        std::optional<std::bitset<CHUNK_AREA> > candidatePoints = MatchStructureConditions(
            appContext, terrainResult, structureResource);

        if (!candidatePoints.has_value()) {
            continue;
        }

        PlaceStructureAtCandidatePoints(appContext, terrainManager, position,
                                        candidatePoints.value(), structureResource);
    }
}

void glimmer::StructurePlacer::PlaceStructureTiles(TerrainManager *terrainManager,
                                                   const StructureInfo &structureInfo,
                                                   const TileVector2D &globalOrigin) {
    const int baseX = globalOrigin.x;
    const int baseY = globalOrigin.y;

    TerrainResult *currentTerrain = nullptr;
    TileVector2D currentChunk = {INT_MIN, INT_MIN};
    for (auto &[tileLayerType, tileMap]: structureInfo.GetStructureMap()) {
        for (auto &[coord, tileResource]: tileMap) {
            const int worldX = baseX + coord.x;
            const int worldY = baseY + coord.y;
            const int chunkX = worldX & ~CHUNK_MASK;
            const int chunkY = worldY & ~CHUNK_MASK;
            const int relativeX = worldX & CHUNK_MASK;
            const int relativeY = worldY & CHUNK_MASK;
            TileVector2D chunkCoord{chunkX, chunkY};
            if (chunkCoord != currentChunk) {
                currentChunk = chunkCoord;
                currentTerrain = terrainManager->GetOrCreateTerrainData(chunkCoord);
            }
            if (currentTerrain == nullptr) {
                continue;
            }
            const int index = relativeY << CHUNK_SHIFT | relativeX;
            currentTerrain->SetTerrainTileStructure(index, &tileResource);
        }
    }
}

std::optional<std::bitset<CHUNK_AREA> > glimmer::StructurePlacer::MatchStructureConditions(
    const AppContext *appContext, TerrainResult *terrainResult, const IStructureResource *structureResource) {
    const size_t totalConditions = structureResource->condition.size();
    if (totalConditions == 0) {
        return std::nullopt;
    }

    std::string resId = Resource::GenerateId(*structureResource);
    std::bitset<CHUNK_AREA> totalBitset;
    bool hasAnyConditionMatched = false;
    const int endIndex = static_cast<int>(totalConditions) - 1;
    ModContext *modContext = appContext->
            GetModContext();
    if (modContext == nullptr) {
        return std::nullopt;
    }
    StructurePlacementConditionsProcessorManager *structurePlacementConditionsProcessorManager = modContext->
            GetStructurePlacementConditionsProcessorManager();
    if (structurePlacementConditionsProcessorManager == nullptr) {
        return std::nullopt;
    }
    StructurePlacementConditionsRegistry *structurePlacementConditionsRegistry = modContext->
            GetStructurePlacementConditionsRegistry();
    if (structurePlacementConditionsRegistry == nullptr) {
        return std::nullopt;
    }

    for (int i = 0; i <= endIndex; ++i) {
        auto &conditionRef = structureResource->condition[i];
        IStructurePlacementConditionsResource *structurePlacementConditionsResource =
                structurePlacementConditionsRegistry->Find(conditionRef.GetPackageId(),
                                                           conditionRef.GetResourceKey());
        if (structurePlacementConditionsResource == nullptr) {
            continue;
        }
        const auto processorType = static_cast<StructureConditionProcessorType>(structurePlacementConditionsResource->
            processorId);
        IStructureConditionProcessor *structureConditionProcessor = structurePlacementConditionsProcessorManager->
                FindConditionProcessors(processorType);
        if (structureConditionProcessor == nullptr) {
            continue;
        }

        std::bitset<CHUNK_AREA> bitset = structureConditionProcessor->Match(
            terrainResult, structurePlacementConditionsResource);

        if (bitset.none()) {
            return std::nullopt;
        }

        if (!hasAnyConditionMatched) {
            totalBitset = bitset;
            hasAnyConditionMatched = true;
        } else {
            totalBitset &= bitset;
        }

        if (totalBitset.none()) {
            return std::nullopt;
        }
    }

    if (!hasAnyConditionMatched || totalBitset.none()) {
        return std::nullopt;
    }

    return totalBitset;
}

int glimmer::StructurePlacer::PlaceStructureAtCandidatePoints(const AppContext *appContext,
                                                              TerrainManager *terrainManager,
                                                              const TileVector2D &position,
                                                              const std::bitset<CHUNK_AREA> &candidatePoints,
                                                              IStructureResource *structureResource) const {
    int markedCount = 0;
    StructureGeneratorManager *structureGeneratorManager = appContext->GetModContext()->GetStructureGeneratorManager();

    for (int i = 0; i < CHUNK_AREA; ++i) {
        if (!candidatePoints.test(i)) {
            continue;
        }

        const int localX = i & CHUNK_MASK;
        const int localY = i >> CHUNK_SHIFT;
        TileVector2D structuralOrigin{localX, localY};
        TileVector2D globalOrigin = position + structuralOrigin;
        std::optional<StructureInfo> structureInfoOptional = structureGeneratorManager->
                Generate(worldContext_, globalOrigin, structureResource);

        if (structureInfoOptional.has_value()) {
            PlaceStructureTiles(terrainManager, structureInfoOptional.value(), globalOrigin);
        }

        ++markedCount;
    }

    return markedCount;
}
