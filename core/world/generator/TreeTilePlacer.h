//
// Created by Cold-Mint on 2026/02/01.
//

#ifndef GLIMMERWORKS_TREETILEPLACER_H
#define GLIMMERWORKS_TREETILEPLACER_H

#include <FastNoiseLite.h>

#include "TilePlacer.h"

namespace glimmer {
    class TreeTilePlacer : public TilePlacer {
        /**
         * Noise used for controlling the generation of trees
         * 用于控制树木生成的噪声
         */
        std::unique_ptr<FastNoiseLite> treeNoise;

    public:
        ~TreeTilePlacer() override = default;

        void SetSeed(int seed) override;

        [[nodiscard]] bool PlaceTileId(AppContext *appContext,
                                       std::array<std::array<ResourceRef, CHUNK_SIZE>, CHUNK_SIZE> &tilesRef,
                                       std::vector<ResourceRef> &tileSet,
                                       std::vector<TileVector2D> &coordinateArray, bool includeSky,
                                       VariableConfig configData) override;

        std::string GetId() override;
    };
}

#endif //GLIMMERWORKS_TREETILEPLACER_H
