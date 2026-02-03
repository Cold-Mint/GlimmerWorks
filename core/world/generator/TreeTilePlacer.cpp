//
// Created by Cold-Mint on 2026/02/01.
//

#include "TreeTilePlacer.h"


void glimmer::TreeTilePlacer::SetSeed(const int seed) {
    if (treeNoise == nullptr) {
        treeNoise = std::make_unique<FastNoiseLite>();
    }
    treeNoise->SetSeed(seed);
}

bool glimmer::TreeTilePlacer::PlaceTileId(AppContext *appContext,
                                          std::array<std::array<ResourceRef, CHUNK_SIZE>, CHUNK_SIZE> &tilesRef,
                                          std::vector<ResourceRef> &tileSet,
                                          std::vector<TileVector2D> &coordinateArray, bool includeSky,
                                          VariableConfig configData) {
    // LogCat::d("TreeTilePlacer includeSky=", includeSky);
    // if (!includeSky) {
    //     //Trees will not be generated in the underground area.
    //     //树木不会生成在地下区域。
    //     LogCat::d("TreeTilePlacer Does not include air.");
    //     return false;
    // }
    // auto leaveIndex = configData.FindVariable("leaveIndex")->AsInt();
    // auto treeIndex = configData.FindVariable("treeIndex")->AsInt();
    // auto &leaveTile = tileSet[leaveIndex];
    // // auto &treeTile = tileSet[treeIndex];
    // for (const auto coordinate: coordinateArray) {
    //     tilesRef[coordinate.x][coordinate.y] = leaveTile;
    // }
    // LogCat::d("TreeTilePlacer success.");
    return true;
}

std::string glimmer::TreeTilePlacer::GetId() {
    return TREE_TILE_PLACER_ID;
}
