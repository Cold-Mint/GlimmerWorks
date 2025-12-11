//
// Created by Cold-Mint on 2025/12/9.
//

#include "TilePlacerManager.h"
#include "TilePlacer.h"

std::unordered_map<std::string, std::unique_ptr<glimmer::TilePlacer> > tilePlacerMap;


void glimmer::TilePlacerManager::RegisterTilePlacer(std::unique_ptr<TilePlacer> tilePlacer) {
    tilePlacerMap[tilePlacer->GetId()] = std::move(tilePlacer);
}

glimmer::TilePlacer *glimmer::TilePlacerManager::GetTilePlacer(const std::string &id) {
    auto it = tilePlacerMap.find(id);
    if (it == tilePlacerMap.end()) {
        return nullptr;
    }
    return it->second.get();
}
