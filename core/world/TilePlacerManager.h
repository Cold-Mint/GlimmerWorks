//
// Created by Cold-Mint on 2025/12/9.
//

#ifndef GLIMMERWORKS_TILEPLACERMANAGER_H
#define GLIMMERWORKS_TILEPLACERMANAGER_H
#include <memory>
#include <string>
#include <unordered_map>

#include "TilePlacer.h"

namespace glimmer {
    class TilePlacerManager {
        std::unordered_map<std::string, std::unique_ptr<TilePlacer> > _tilePlacerMap;

    public:
        void RegisterTilePlacer(std::unique_ptr<TilePlacer> tilePlacer);

        TilePlacer *GetTilePlacer(const std::string &id);
    };
}

#endif //GLIMMERWORKS_TILEPLACERMANAGER_H
