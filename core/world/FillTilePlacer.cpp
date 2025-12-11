//
// Created by Cold-Mint on 2025/12/11.
//

#include "FillTilePlacer.h"

#include "../Constants.h"
#include "../mod/ResourceLocator.h"
#include "../scene/AppContext.h"
#include "nlohmann/json.hpp"

glimmer::FillTilePlacer::~FillTilePlacer() = default;

bool glimmer::FillTilePlacer::PlaceTileId(AppContext *appContext,
                                          std::array<std::array<ResourceRef, CHUNK_SIZE>, CHUNK_SIZE> &tilesRef,
                                          std::vector<ResourceRef> &tileSet,
                                          std::vector<TileVector2D> &coordinateArray,
                                          std::optional<nlohmann::json> configData) {
    if (tileSet.empty()) {
        return false;
    }
    const auto &tileRefObj = tileSet.at(0);
    for (const auto coordinate: coordinateArray) {
        tilesRef[coordinate.x][coordinate.y] = tileRefObj;
    }
    return false;
}

std::string glimmer::FillTilePlacer::GetId() {
    return FULL_TILE_PLACER_ID;
}
