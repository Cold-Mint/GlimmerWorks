//
// Created by coldmint on 2026/4/6.
//

#ifndef GLIMMERWORKS_CAVEBIOMEDECORATOR_H
#define GLIMMERWORKS_CAVEBIOMEDECORATOR_H
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


#endif //GLIMMERWORKS_CAVEBIOMEDECORATOR_H
