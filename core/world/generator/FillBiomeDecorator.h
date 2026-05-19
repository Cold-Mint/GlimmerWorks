//
// Created by Cold-Mint on 2025/12/11.
//

#pragma once
#include "BiomeDecorator.h"

namespace glimmer {
    class FillBiomeDecorator : public BiomeDecorator<FillBiomeDecoratorResource> {
        void DecorationImp(WorldContext *worldContext, TerrainResult *terrainResult,
                           FillBiomeDecoratorResource *decoratorResource, BiomeResource *biomeResource,
                           std::unordered_map<TileLayerType, std::array<ResourceRef, CHUNK_AREA> > *
                           tilesRefMap) override;

    public:
        BiomeDecoratorType GetBiomeDecoratorType() override;
    };
}
