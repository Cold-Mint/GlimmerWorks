//
// Created by Cold-Mint on 2025/12/11.
//

#ifndef GLIMMERWORKS_FILLTILEPLACER_H
#define GLIMMERWORKS_FILLTILEPLACER_H
#include "TilePlacer.h"

namespace glimmer {
    class FillTilePlacer : public TilePlacer {
    public:
        ~FillTilePlacer() override;

        [[nodiscard]] bool PlaceTileId(AppContext *appContext,
            std::array<std::array<ResourceRef, CHUNK_SIZE>, CHUNK_SIZE> &tilesRef, std::vector<ResourceRef> &tileSet,
            std::vector<TileVector2D> &coordinateArray, bool includeSky, VariableConfig configData) override;

        std::string GetId() override;
    };
}

#endif //GLIMMERWORKS_FILLTILEPLACER_H
