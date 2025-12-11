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

        [[nodiscard]] bool Place(AppContext *appContext, TileLayerComponent *tileLayerComponent,
            std::vector<std::string> tileSet, std::vector<TileVector2D> coordinateArray, nlohmann::json json) override;

        std::string GetId() override;
    };
}

#endif //GLIMMERWORKS_FILLTILEPLACER_H
