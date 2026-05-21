//
// Created by Cold-Mint on 2026/2/13.
//

#include "StructureGeneratorManager.h"

void glimmer::StructureGeneratorManager::SetWorldSeed(const int worldSeed) {
    for (const auto &structureGenerator: structureGeneratorMap_) {
        structureGenerator.second->SetWorldSeed(worldSeed);
    }
}

void glimmer::StructureGeneratorManager::RegisterStructureGenerator(
    std::unique_ptr<IStructureGenerator> structureGenerator) {
    const StructureGeneratorType type = structureGenerator->GetStructureGeneratorType();
    structureGeneratorMap_.emplace(type, std::move(structureGenerator));
}

std::optional<glimmer::StructureInfo> glimmer::StructureGeneratorManager::Generate(const TileVector2D structuralOrigin,
    IStructureResource *structureResource) {
    const auto type = static_cast<StructureGeneratorType>(structureResource->generatorId);
    if (!structureGeneratorMap_.contains(type)) {
        return std::nullopt;
    }
    return structureGeneratorMap_[type]->Generate(structuralOrigin, structureResource);
}
