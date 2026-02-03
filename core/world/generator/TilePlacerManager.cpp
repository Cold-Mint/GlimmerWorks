//
// Created by Cold-Mint on 2025/12/9.
//

#include "TilePlacerManager.h"
#include "TilePlacer.h"

void glimmer::TilePlacerManager::RegisterTilePlacer(std::unique_ptr<TilePlacer> tilePlacer) {
    tilePlacerMap_[tilePlacer->GetId()] = std::move(tilePlacer);
}

glimmer::TilePlacer *glimmer::TilePlacerManager::GetTilePlacer(const std::string &id) {
    const auto it = tilePlacerMap_.find(id);
    if (it == tilePlacerMap_.end()) {
        return nullptr;
    }
    return it->second.get();
}

void glimmer::TilePlacerManager::SetSeed(const uint64_t seed) const {
    for (const auto &it: tilePlacerMap_) {
        it.second->SetSeed(seed);
    }
}
