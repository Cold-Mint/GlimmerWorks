//
// Created by Cold-Mint on 2026/4/6.
//

#pragma once
#include "BiomeDecorator.h"


namespace glimmer {
    class MineralBiomeDecorator : public BiomeDecorator<MineralBiomeDecoratorResource> {
        void DecorationImp(WorldContext *worldContext, TerrainResult *terrainResult,
                           MineralBiomeDecoratorResource *decoratorResource, BiomeResource *biomeResource,
                           std::unordered_map<TileLayerType, std::array<ResourceRef, CHUNK_AREA> > *
                           tilesRefMap) override;

    public:
        BiomeDecoratorType GetBiomeDecoratorType() override;
    };
}
