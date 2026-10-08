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
#include "core/math/CoordinateTransformer.h"
#include "core/mod/Resource.h"
#include "core/mod/dataPack/TileResourceManager.h"


void glimmer::TileRefResolver::WriteWaterResourceRef(const TileLayerType layerType, ResourceRef& resourceRef)
{
    //The ground layer is set to water, while the other layers are set to air.
    //地面层设置为水，其他层设置为空气。
    if (layerType == TileLayerType::Ground)
    {
        resourceRef.SetResourceType(RESOURCE_TILE);
        resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
        resourceRef.SetResourceKey(TILE_ID_WATER);
    }
    else
    {
        TileResourceManager::WriteAirResourceRef(
            layerType, resourceRef);
    }
}

void glimmer::TileRefResolver::WriteAirResourceRef(const TileLayerType layerType, ResourceRef& resourceRef)
{
    TileResourceManager::WriteAirResourceRef(
        layerType, resourceRef);
}

void glimmer::TileRefResolver::WriteBedRockResourceRef(const TileLayerType layerType, ResourceRef& resourceRef)
{
    //The ground layer is equipped with bedrock, while the other layers are fitted with void walls.
    //地面层设置基岩，其他层设置虚空墙壁。
    if (layerType == TileLayerType::Ground)
    {
        resourceRef.SetResourceType(RESOURCE_TILE);
        resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
        resourceRef.SetResourceKey(TILE_ID_BEDROCK);
    }
    else
    {
        resourceRef.SetResourceType(RESOURCE_TILE);
        resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
        resourceRef.SetResourceKey(TILE_ID_VOID_WALL);
    }
}

void glimmer::TileRefResolver::WriteVoidResourceRef(const TileLayerType layerType, ResourceRef& resourceRef)
{
    if (layerType == TileLayerType::Ground)
    {
        resourceRef.SetResourceType(RESOURCE_TILE);
        resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
        resourceRef.SetResourceKey(TILE_ID_VOID);
    }
    else
    {
        resourceRef.SetResourceType(RESOURCE_TILE);
        resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
        resourceRef.SetResourceKey(TILE_ID_VOID_WALL);
    }
}

void glimmer::TileRefResolver::Initialize(const TerrainResult* terrainResult,
                                          const ChunkVertexVector2D& chunkVertexVector2D,
                                          std::unordered_map<TileLayerType, std::array<ResourceRef, CHUNK_AREA>>&
                                          tilesRefMap,
                                          std::unordered_set<BiomeResource*>& biomeResourcesSet)
{
    for (int localX = 0; localX < CHUNK_SIZE; ++localX)
    {
        for (int localY = 0; localY < CHUNK_SIZE; ++localY)
        {
            ChunkRelativeVector2D chunkRelativeVector2D(localX, localY);
            const auto& terrainTileResult = terrainResult->QueryTerrain(CoordinateTransformer::TileToTerrainRelative(
                CoordinateTransformer::ChunkRelativeToTile(
                    chunkVertexVector2D, chunkRelativeVector2D)));
            const int idx = localY * CHUNK_SIZE + localX;
            const TerrainResultType terrainType = terrainTileResult.GetTerrainType();
            if (terrainType == TerrainResultType::VOID)
            {
                for (auto& [layerType, refsArray] : tilesRefMap)
                {
                    WriteVoidResourceRef(layerType, refsArray[idx]);
                }
            }
            if (terrainType == TerrainResultType::AIR)
            {
                for (auto& [layerType, refsArray] : tilesRefMap)
                {
                    WriteAirResourceRef(layerType, refsArray[idx]);
                }
            }
            if (terrainType == TerrainResultType::WATER)
            {
                for (auto& [layerType, refsArray] : tilesRefMap)
                {
                    WriteWaterResourceRef(layerType, refsArray[idx]);
                }
            }
            if (terrainType == TerrainResultType::SOLID)
            {
                if (BiomeResource* biomeResource = terrainTileResult.GetBiomeResource(); biomeResource != nullptr)
                {
                    biomeResourcesSet.insert(biomeResource);
                }
            }
            if (terrainType == TerrainResultType::STRUCTURE)
            {
                for (const auto& [structureLayerType, structureResRef] : terrainTileResult.GetStructureResRefs())
                {
                    tilesRefMap[structureLayerType][idx] = structureResRef;
                }
            }
            if (terrainType == TerrainResultType::BEDROCK)
            {
                for (auto& [layerType, refsArray] : tilesRefMap)
                {
                    WriteBedRockResourceRef(layerType, refsArray[idx]);
                }
            }
        }
    }
}
