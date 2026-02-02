//
// Created by Cold-Mint on 2026/02/01.
//

#include "TreeTilePlacer.h"
#include "../Constants.h"

void glimmer::TreeTilePlacer::SetSeed(const uint64_t seed) {
    if (treeNoise == nullptr) {
        treeNoise = std::make_unique<FastNoiseLite>();
    }
    treeNoise->SetSeed(seed);
}

bool glimmer::TreeTilePlacer::PlaceTileId(AppContext *appContext,
    std::array<std::array<ResourceRef, CHUNK_SIZE>, CHUNK_SIZE> &tilesRef, std::vector<ResourceRef> &tileSet,
    std::vector<TileVector2D> &coordinateArray, bool includeSky, VariableConfig configData) {
    if (!includeSky) {
        //Trees will not be generated in the underground area.
        //树木不会生成在地下区域。
        return false;
    }
    return false;
}


std::string glimmer::TreeTilePlacer::GetId() {
    return TREE_TILE_PLACER_ID;
}
