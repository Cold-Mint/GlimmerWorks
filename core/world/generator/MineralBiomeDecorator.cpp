//
// Created by coldmint on 2026/4/6.
//

#include "MineralBiomeDecorator.h"


void glimmer::MineralBiomeDecorator::DecorationImp(WorldContext *worldContext, TerrainResult *terrainResult,
    MineralBiomeDecoratorResource *decoratorResource, BiomeResource *biomeResource,
    std::array<ResourceRef, CHUNK_AREA> &tilesRef) {

}

glimmer::BiomeDecoratorType glimmer::MineralBiomeDecorator::GetBiomeDecoratorType() {
    return BiomeDecoratorType::MINERAL;
}
