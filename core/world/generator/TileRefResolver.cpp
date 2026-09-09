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
#include "TileRefResolver.h"

#include "TerrainResult.h"
#include "TerrainResultType.h"
#include "TerrainTileResult.h"
#include "core/mod/Resource.h"
#include "core/mod/dataPack/TileResourceManager.h"

glimmer::TerrainTileRefs glimmer::TerrainTileRefs::Create() {
    TerrainTileRefs refs;
    refs.water.SetResourceType(RESOURCE_TILE);
    refs.water.SetPackageId(RESOURCE_REF_CORE);
    refs.water.SetSelfPackageId(RESOURCE_REF_CORE);
    refs.water.SetResourceKey(TILE_ID_WATER);
    refs.voidWall.SetResourceType(RESOURCE_TILE);
    refs.voidWall.SetPackageId(RESOURCE_REF_CORE);
    refs.voidWall.SetSelfPackageId(RESOURCE_REF_CORE);
    refs.voidWall.SetResourceKey(TILE_ID_VOID_WALL);
    refs.bedrock.SetResourceType(RESOURCE_TILE);
    refs.bedrock.SetPackageId(RESOURCE_REF_CORE);
    refs.bedrock.SetSelfPackageId(RESOURCE_REF_CORE);
    refs.bedrock.SetResourceKey(TILE_ID_BEDROCK);
    return refs;
}

void glimmer::TileRefResolver::Initialize(const TerrainResult *terrainResult, const TerrainTileRefs &tileRefs,
                                          std::unordered_map<TileLayerType, std::array<ResourceRef, CHUNK_AREA> > &
                                          tilesRefMap,
                                          std::unordered_set<BiomeResource *> &biomeResourcesSet) {
    for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
        for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
            const int idx = localY * CHUNK_SIZE + localX;
            const auto &terrainTileResult = terrainResult->QueryTerrain(localX, localY);
            if (terrainTileResult.terrainType == TerrainResultType::BEDROCK) {
                tilesRefMap[TileLayerType::BackGround][idx] = tileRefs.voidWall;
            } else {
                tilesRefMap[TileLayerType::BackGround][idx] = TileResourceManager::GetAirResourceRef(
                    TileLayerType::BackGround);
            }
            SetTileRefForTerrainType(idx, terrainTileResult, tilesRefMap, biomeResourcesSet, tileRefs.water,
                                     tileRefs.bedrock);
        }
    }
}

void glimmer::TileRefResolver::SetTileRefForTerrainType(const int idx,
                                                        const TerrainTileResult &terrainTileResult,
                                                        std::unordered_map<TileLayerType, std::array<ResourceRef,
                                                            CHUNK_AREA> > &tilesRefMap,
                                                        std::unordered_set<BiomeResource *> &biomeResourcesSet,
                                                        const ResourceRef &waterTileRef,
                                                        const ResourceRef &bedrockTileRef) {
    using enum TerrainResultType;
    using enum TileLayerType;
    switch (terrainTileResult.terrainType) {
        case AIR:
            tilesRefMap[Ground][idx] = TileResourceManager::GetAirResourceRef(Ground);
            break;
        case WATER:
            tilesRefMap[Ground][idx] = waterTileRef;
            break;
        case BEDROCK:
            tilesRefMap[Ground][idx] = bedrockTileRef;
            break;
        case STRUCTURE:
            tilesRefMap[Ground][idx] = terrainTileResult.structureResRef;
            break;
        case SOLID:
            tilesRefMap[Ground][idx] = TileResourceManager::GetAirResourceRef(Ground);
            if (terrainTileResult.biomeResource != nullptr) {
                biomeResourcesSet.insert(terrainTileResult.biomeResource);
            }
            break;
    }
}
