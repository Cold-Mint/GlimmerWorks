//
// Created by coldmint on 2026/2/4.
//

#ifndef GLIMMERWORKS_TERRAINTILERESULT_H
#define GLIMMERWORKS_TERRAINTILERESULT_H
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
};


#endif //GLIMMERWORKS_TERRAINTILERESULT_H
