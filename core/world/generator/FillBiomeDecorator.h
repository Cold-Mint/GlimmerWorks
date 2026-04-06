//
// Created by Cold-Mint on 2025/12/11.
//

#ifndef GLIMMERWORKS_FILLTILEPLACER_H
#define GLIMMERWORKS_FILLTILEPLACER_H
#include "BiomeDecorator.h"

namespace glimmer {
    class FillBiomeDecorator : public BiomeDecorator<FillBiomeDecoratorResource> {
        void DecorationImp(WorldContext *worldContext, TerrainResult *terrainResult,
            FillBiomeDecoratorResource *decoratorResource, BiomeResource *biomeResource,
            std::array<ResourceRef, CHUNK_AREA> &tilesRef) override;

    public:
        BiomeDecoratorType GetBiomeDecoratorType() override;
    };
}

#endif //GLIMMERWORKS_FILLTILEPLACER_H
