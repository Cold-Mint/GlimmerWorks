//
// Created by Cold-Mint on 2025/12/11.
//

#include "FillTilePlacer.h"

#include "../Constants.h"
#include "nlohmann/json.hpp"

glimmer::FillTilePlacer::~FillTilePlacer() = default;

bool glimmer::FillTilePlacer::Place(AppContext *appContext, TileLayerComponent *tileLayerComponent,
                                    std::vector<std::string> tileSet, std::vector<TileVector2D> coordinateArray,
                                    nlohmann::json json) {
    return false;
}


std::string glimmer::FillTilePlacer::GetId() {
    return FULL_TILE_PLACER_ID;
}
