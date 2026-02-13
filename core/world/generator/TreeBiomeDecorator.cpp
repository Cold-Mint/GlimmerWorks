//
// Created by Cold-Mint on 2026/02/01.
//

#include "TreeBiomeDecorator.h"


void glimmer::TreeBiomeDecorator::SetWorldSeed(const int seed) {
    if (treeNoise == nullptr) {
        treeNoise = std::make_unique<FastNoiseLite>();
    }
    treeNoise->SetSeed(seed);
}

void glimmer::TreeBiomeDecorator::Decoration(
    WorldContext *worldContext,
    TerrainResult *terrainResult,
    BiomeDecoratorResource *biomeDecoratorResource,
    BiomeResource *biomeResource,
    std::array<std::array<ResourceRef, CHUNK_SIZE>, CHUNK_SIZE> &tilesRef) {
    if (treeNoise == nullptr) {
        return;
    }

}


std::string glimmer::TreeBiomeDecorator::GetId() {
    return TREE_BIOME_DECORATOR;
}
