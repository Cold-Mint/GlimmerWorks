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
    /**
     * BiomeDecorator
     * 生物群系装饰器
     */
    class BiomeDecorator {
    public:
        virtual ~BiomeDecorator();


        virtual void SetWorldSeed(int seed);

        virtual void Decoration(WorldContext *worldContext, TerrainResult *terrainResult,
                                BiomeDecoratorResource *biomeDecoratorResource,
                                BiomeResource *biomeResource,
                                std::array<ResourceRef, CHUNK_AREA> &tilesRef) = 0;

        /**
         * Get the id of the tile placement device
         * 获取瓦片放置器的Id
         * @return
         */
        virtual std::string GetId() = 0;
    };
}

#endif //GLIMMERWORKS_TILEPLACER_H
