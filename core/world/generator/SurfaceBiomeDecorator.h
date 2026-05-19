//
// Created by Cold-Mint on 2026/2/2.
//

#pragma once
#include "BiomeDecorator.h"

namespace glimmer {
    class SurfaceBiomeDecorator : public BiomeDecorator<SurfaceBiomeDecoratorResource> {
        void DecorationImp(WorldContext *worldContext, TerrainResult *terrainResult,
                           SurfaceBiomeDecoratorResource *decoratorResource, BiomeResource *biomeResource,
                           std::unordered_map<TileLayerType, std::array<ResourceRef, CHUNK_AREA> > *
                           tilesRefMap) override;

    public:
        BiomeDecoratorType GetBiomeDecoratorType() override;
    };
}
