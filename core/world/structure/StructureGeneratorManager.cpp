//
// Created by coldmint on 2026/2/13.
//

#include "StructureGeneratorManager.h"

void glimmer::StructureGeneratorManager::SetWorldSeed(const int worldSeed) {
    for (auto &structureGenerator: structureGeneratorMap_) {
        structureGenerator.second->SetWorldSeed(worldSeed);
    }
}

void glimmer::StructureGeneratorManager::RegisterStructureGenerator(
    std::unique_ptr<IStructureGenerator> structureGenerator) {
    const std::string id = structureGenerator->GetStructureGeneratorId();
    structureGeneratorMap_.emplace(id, std::move(structureGenerator));
}

std::optional<glimmer::StructureInfo> glimmer::StructureGeneratorManager::Generate(TileVector2D startPosition,
    StructureResource *structureResource) {
    const std::string &generatorId = structureResource->generatorId;
    if (!structureGeneratorMap_.contains(generatorId)) {
        return std::nullopt;
    }
    return structureGeneratorMap_[generatorId]->Generate(startPosition, structureResource);
}
