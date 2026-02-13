//
// Created by Cold-Mint on 2025/12/9.
//

#include "BiomeDecoratorManager.h"
#include "BiomeDecorator.h"


void glimmer::BiomeDecoratorManager::RegisterBiomeDecorator(std::unique_ptr<BiomeDecorator> biomeDecorator) {
    biomeDecoratorMap_[biomeDecorator->GetId()] = std::move(biomeDecorator);
}

glimmer::BiomeDecorator *glimmer::BiomeDecoratorManager::GetBiomeDecorator(const std::string &id) {
    const auto it = biomeDecoratorMap_.find(id);
    if (it == biomeDecoratorMap_.end()) {
        return nullptr;
    }
    return it->second.get();
}

void glimmer::BiomeDecoratorManager::SetWorldSeed(const int worldSeed) const {
    for (const auto &it: biomeDecoratorMap_) {
        it.second->SetWorldSeed(worldSeed);
    }
}
