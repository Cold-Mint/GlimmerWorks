//
// Created by coldmint on 2026/2/4.
//

#include "TerrainResult.h"

void glimmer::TerrainResult::AddTile(TileVector2D vector2d, TerrainTileResult result) {
    terrainTileResult_[vector2d] = result;
}

std::unordered_map<TileVector2D, TerrainTileResult, glimmer::Vector2DIHash> glimmer::TerrainResult::
GetResult() const {
    return terrainTileResult_;
}
