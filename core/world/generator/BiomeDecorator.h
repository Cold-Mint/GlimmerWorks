//
// Created by Cold-Mint on 2025/12/9.
//

#pragma once

#include "Chunk.h"
#include "TerrainResult.h"
#include "core/Constants.h"
#include "core/mod/Resource.h"
#include "core/mod/ResourceRef.h"

namespace glimmer {
    class IBiomeDecorator {
    protected:
        int worldSeed_ = 0;

    public:
        virtual ~IBiomeDecorator() = default;

        void SetWorldSeed(int seed);

        virtual void Decoration(WorldContext *worldContext,
                                TerrainResult *terrainResult,
                                IBiomeDecoratorResource *decoratorResource,
                                BiomeResource *biomeResource,
                                std::unordered_map<TileLayerType, std::array<ResourceRef, CHUNK_AREA> > *tilesRefMap) =
        0;

        virtual BiomeDecoratorType GetBiomeDecoratorType() = 0;
    };

    template<typename ResourceT>
    class BiomeDecorator : public IBiomeDecorator {
        virtual void DecorationImp(WorldContext *worldContext, TerrainResult *terrainResult,
                                   ResourceT *decoratorResource, BiomeResource *biomeResource,
                                   std::unordered_map<TileLayerType, std::array<ResourceRef, CHUNK_AREA> > *tilesRefMap)
        = 0;

    public:
        void Decoration(WorldContext *worldContext, TerrainResult *terrainResult,
                        IBiomeDecoratorResource *decoratorResource, BiomeResource *biomeResource,
                        std::unordered_map<TileLayerType, std::array<ResourceRef, CHUNK_AREA> > *tilesRefMap) override {
            auto *concreteResource = static_cast<ResourceT *>(decoratorResource);
            DecorationImp(worldContext, terrainResult, concreteResource, biomeResource, tilesRefMap);
        }
    };
}
