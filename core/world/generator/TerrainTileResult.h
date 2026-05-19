//
// Created by Cold-Mint on 2026/2/4.
//

#pragma once
#include "TerrainResultType.h"
#include "core/mod/Resource.h"
#include "core/math/Vector2DI.h"


/**
 * TerrainTileResult
 * 瓦片地形结果
 */
struct TerrainTileResult {
    TileVector2D world;
    glimmer::TerrainResultType terrainType = glimmer::AIR;
    glimmer::BiomeResource *biomeResource = nullptr;
    glimmer::ResourceRef resRef;
};
