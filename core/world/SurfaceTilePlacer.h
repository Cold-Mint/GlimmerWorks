//
// Created by coldmint on 2026/2/2.
//

#ifndef GLIMMERWORKS_SURFACETILEPLACER_H
#define GLIMMERWORKS_SURFACETILEPLACER_H
#include "TilePlacer.h"

namespace glimmer {
    class SurfaceTilePlacer : public TilePlacer {
    public:
        [[nodiscard]] bool PlaceTileId(AppContext *appContext,
                                       std::array<std::array<ResourceRef, CHUNK_SIZE>, CHUNK_SIZE> &tilesRef,
                                       std::vector<ResourceRef> &tileSet,
                                       std::vector<TileVector2D> &coordinateArray, bool includeSky,
                                       VariableConfig configData) override;

        std::string GetId() override;
    };
}

#endif //GLIMMERWORKS_SURFACETILEPLACER_H
