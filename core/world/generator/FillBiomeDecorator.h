//
// Created by Cold-Mint on 2025/12/11.
//

#ifndef GLIMMERWORKS_FILLTILEPLACER_H
#define GLIMMERWORKS_FILLTILEPLACER_H
#include "BiomeDecorator.h"

namespace glimmer {
    class FillBiomeDecorator : public BiomeDecorator {
    public:
        void Decoration(WorldContext *worldContext, TerrainResult *terrainResult,
                        BiomeDecoratorResource *biomeDecoratorResource, BiomeResource *biomeResource,
                        std::array<std::array<ResourceRef, CHUNK_SIZE>, CHUNK_SIZE> &tilesRef) override;


        std::string GetId() override;
    };
}

#endif //GLIMMERWORKS_FILLTILEPLACER_H
