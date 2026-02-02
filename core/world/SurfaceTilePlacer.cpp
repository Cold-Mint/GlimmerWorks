//
// Created by coldmint on 2026/2/2.
//

#include "SurfaceTilePlacer.h"

bool glimmer::SurfaceTilePlacer::PlaceTileId(AppContext *appContext,
                                             std::array<std::array<ResourceRef, CHUNK_SIZE>, CHUNK_SIZE> &tilesRef,
                                             std::vector<ResourceRef> &tileSet,
                                             std::vector<TileVector2D> &coordinateArray, bool includeSky,
                                             VariableConfig configData) {
    if (!includeSky) {
        return false;
    }
    const auto &tileRefObj = tileSet.at(0);

    // For each x, the corresponding maximum y value is given. -1 indicates that it does not exist.
    // 每个 x 对应的最高 y，-1 表示不存在
    std::array<int, CHUNK_SIZE> maxYByX{};
    maxYByX.fill(-1);

    // First traversal: Find the highest y
    // 第一次遍历：找最高 y
    for (const auto &coord: coordinateArray) {
        int &maxY = maxYByX[coord.x];
        if (coord.y > maxY) {
            maxY = coord.y;
        }
    }

    // Second time: Placement
    // 第二次：放置
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        int y = maxYByX[x];
        if (y < 0) {
            continue;
        }

        if (ResourceRef &oldTile = tilesRef[x][y];
            oldTile.GetPackageId() == RESOURCE_REF_CORE &&
            oldTile.GetResourceKey() == TILE_ID_AIR) {
            tilesRef[x][y] = tileRefObj;
        }
    }

    return true;
}

std::string glimmer::SurfaceTilePlacer::GetId() {
    return SURFACE_PLACER_ID;
}
