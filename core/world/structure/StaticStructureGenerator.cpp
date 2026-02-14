//
// Created by coldmint on 2026/2/7.
//

#include "StaticStructureGenerator.h"

glimmer::StructureInfo glimmer::StaticStructureGenerator::Generate(TileVector2D startPosition,
    StructureResource *structureResource) {
    return StructureInfo(structureResource->width, &structureResource->data, startPosition);
}

std::string glimmer::StaticStructureGenerator::GetStructureGeneratorId() {
    return STRUCTURE_GENERATOR_ID_STATIC;
}
