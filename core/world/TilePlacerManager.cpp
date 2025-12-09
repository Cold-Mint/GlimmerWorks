//
// Created by Cold-Mint on 2025/12/9.
//

#include "TilePlacerManager.h"
#include "TilePlacer.h"

void glimmer::TilePlacerManager::RegisterTilePlacer(std::unique_ptr<TilePlacer> tilePlacer) {
    _tilePlacerMap[tilePlacer->GetId()] = std::move(tilePlacer);
}

glimmer::TilePlacer *glimmer::TilePlacerManager::GetTilePlacer(const std::string &id) {
    auto it = _tilePlacerMap.find(id);
    if (it == _tilePlacerMap.end()) {
        return nullptr;
    }
    return it->second.get();
}
