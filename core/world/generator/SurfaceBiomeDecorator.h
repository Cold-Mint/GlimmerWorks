//
// Created by coldmint on 2026/2/2.
//

#ifndef GLIMMERWORKS_SURFACETILEPLACER_H
#define GLIMMERWORKS_SURFACETILEPLACER_H
#include "BiomeDecorator.h"

namespace glimmer {
    class SurfaceBiomeDecorator : public BiomeDecorator {
    public:
        void Decoration(WorldContext *worldContext, TerrainResult *terrainResult,
            BiomeDecoratorResource *biomeDecoratorResource, BiomeResource *biomeResource,
            std::array<ResourceRef, CHUNK_AREA> &tilesRef) override;

        std::string GetId() override;
    };
}

#endif //GLIMMERWORKS_SURFACETILEPLACER_H
