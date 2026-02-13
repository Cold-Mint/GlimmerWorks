//
// Created by coldmint on 2026/2/13.
//

#include "StructureGeneratorManager.h"

void glimmer::StructureGeneratorManager::RegisterStructureGenerator(
    std::unique_ptr<IStructureGenerator> structureGenerator) {
    const std::string id = structureGenerator->GetStructureGeneratorId();
    structureGeneratorMap_.emplace(id, std::move(structureGenerator));
}
