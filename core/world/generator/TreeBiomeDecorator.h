//
// Created by Cold-Mint on 2026/02/01.
//

#ifndef GLIMMERWORKS_TREETILEPLACER_H
#define GLIMMERWORKS_TREETILEPLACER_H

#include <FastNoiseLite.h>

#include "BiomeDecorator.h"

namespace glimmer {
    class TreeBiomeDecorator : public BiomeDecorator {
        /**
         * Noise used for controlling the generation of trees
         * 用于控制树木生成的噪声
         */
        std::unique_ptr<FastNoiseLite> treeNoise;

    public:
        ~TreeBiomeDecorator() override = default;

        void SetWorldSeed(int seed) override;

       void Decoration(WorldContext *worldContext, TerrainResult *terrainResult,
            BiomeDecoratorResource *biomeDecoratorResource, BiomeResource *biomeResource,
            std::array<std::array<ResourceRef, CHUNK_SIZE>, CHUNK_SIZE> &tilesRef) override;

        std::string GetId() override;
    };
}

#endif //GLIMMERWORKS_TREETILEPLACER_H
