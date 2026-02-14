//
// Created by coldmint on 2026/2/7.
//

#include "TreeStructureGenerator.h"


glimmer::StructureInfo glimmer::TreeStructureGenerator::Generate(TileVector2D startPosition,
                                                                 StructureResource *structureResource) {
    return StructureInfo(structureResource->width, &structureResource->data, startPosition);
}

std::string glimmer::TreeStructureGenerator::GetStructureGeneratorId() {
    return STRUCTURE_GENERATOR_ID_TREE;
}
