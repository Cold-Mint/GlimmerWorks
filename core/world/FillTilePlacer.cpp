//
// Created by Cold-Mint on 2025/12/11.
//

#include "FillTilePlacer.h"

#include "../Constants.h"
#include "../scene/AppContext.h"

glimmer::FillTilePlacer::~FillTilePlacer() = default;

bool glimmer::FillTilePlacer::PlaceTileId(AppContext *appContext,
                                          std::array<std::array<ResourceRef, CHUNK_SIZE>, CHUNK_SIZE> &tilesRef,
                                          std::vector<ResourceRef> &tileSet,
                                          std::vector<TileVector2D> &coordinateArray, bool includeSky,
                                          VariableConfig configData) {
    const auto &tileRefObj = tileSet.at(0);
    for (const auto coordinate: coordinateArray) {
        if (ResourceRef &oldTile = tilesRef[coordinate.x][coordinate.y]; oldTile.GetPackageId() == RESOURCE_REF_CORE && oldTile.GetResourceKey() == TILE_ID_AIR) {
            //Only filled in the air.
            //只在空气处填充。
            tilesRef[coordinate.x][coordinate.y] = tileRefObj;
        }
    }
    return true;
}

std::string glimmer::FillTilePlacer::GetId() {
    return FULL_TILE_PLACER_ID;
}
