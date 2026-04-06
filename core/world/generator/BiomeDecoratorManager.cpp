//
// Created by Cold-Mint on 2025/12/9.
//

#include "BiomeDecoratorManager.h"
#include "BiomeDecorator.h"


void glimmer::BiomeDecoratorManager::RegisterBiomeDecorator(std::unique_ptr<IBiomeDecorator> biomeDecorator) {
    biomeDecoratorMap_[biomeDecorator->GetBiomeDecoratorType()] = std::move(biomeDecorator);
}

glimmer::IBiomeDecorator *glimmer::BiomeDecoratorManager::GetBiomeDecorator(BiomeDecoratorType biomeDecoratorType) {
    const auto it = biomeDecoratorMap_.find(biomeDecoratorType);
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
