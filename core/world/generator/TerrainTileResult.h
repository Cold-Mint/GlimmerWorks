//
// Created by coldmint on 2026/2/4.
//

#ifndef GLIMMERWORKS_TERRAINTILERESULT_H
#define GLIMMERWORKS_TERRAINTILERESULT_H
#include "TerrainResultType.h"
#include "core/mod/Resource.h"


/**
 * TerrainTileResult
 * 瓦片地形结果
 */
struct TerrainTileResult {
    glimmer::TerrainResultType terrainType = glimmer::AIR;
    glimmer::BiomeResource *biomeResource = nullptr;
};


#endif //GLIMMERWORKS_TERRAINTILERESULT_H
