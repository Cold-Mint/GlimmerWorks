//
// Created by Cold-Mint on 2025/12/9.
//

#ifndef GLIMMERWORKS_TILEPLACER_H
#define GLIMMERWORKS_TILEPLACER_H
#include <string>

#include "Chunk.h"
#include "TerrainResult.h"
#include "../../Constants.h"
#include "../../mod/Resource.h"
#include "../../mod/ResourceRef.h"

namespace glimmer {
    class IBiomeDecorator {
        int worldSeed_ = 0;

    public:
        virtual ~IBiomeDecorator() = default;

        void SetWorldSeed(int seed);

        virtual void Decoration(WorldContext *worldContext,
                                TerrainResult *terrainResult,
                                IBiomeDecoratorResource *decoratorResource,
                                BiomeResource *biomeResource,
                                std::array<ResourceRef, CHUNK_AREA> &tilesRef) = 0;

        virtual BiomeDecoratorType GetBiomeDecoratorType() = 0;
    };

    template<typename ResourceT>
    class BiomeDecorator : public IBiomeDecorator {
        virtual void DecorationImp(WorldContext *worldContext, TerrainResult *terrainResult,
                                   ResourceT *decoratorResource, BiomeResource *biomeResource,
                                   std::array<ResourceRef, CHUNK_AREA> &tilesRef) = 0;

    public:
        void Decoration(WorldContext *worldContext, TerrainResult *terrainResult,
                        IBiomeDecoratorResource *decoratorResource, BiomeResource *biomeResource,
                        std::array<ResourceRef, CHUNK_AREA> &tilesRef) override {
            auto *concreteResource = static_cast<ResourceT *>(decoratorResource);
            DecorationImp(worldContext, terrainResult, concreteResource, biomeResource, tilesRef);
        }
    };
}

#endif //GLIMMERWORKS_TILEPLACER_H
