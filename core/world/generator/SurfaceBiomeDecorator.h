//
// Created by Cold-Mint on 2026/2/2.
//

#ifndef GLIMMERWORKS_SURFACETILEPLACER_H
#define GLIMMERWORKS_SURFACETILEPLACER_H
#include "BiomeDecorator.h"

namespace glimmer {
    class SurfaceBiomeDecorator : public BiomeDecorator<SurfaceBiomeDecoratorResource> {
        void DecorationImp(WorldContext *worldContext, TerrainResult *terrainResult,
                           SurfaceBiomeDecoratorResource *decoratorResource, BiomeResource *biomeResource,
                           std::array<ResourceRef, CHUNK_AREA> &tilesRef) override;

    public:
        BiomeDecoratorType GetBiomeDecoratorType() override;
    };
}

#endif //GLIMMERWORKS_SURFACETILEPLACER_H
