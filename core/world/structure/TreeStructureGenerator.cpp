//
// Created by coldmint on 2026/2/7.
//

#include "TreeStructureGenerator.h"

glimmer::StructureInfo glimmer::TreeStructureGenerator::Generate(TileVector2D startPosition) {
    return StructureInfo(1, {}, startPosition);
}

std::string glimmer::TreeStructureGenerator::GetStructureGeneratorId() {
    return STRUCTURE_GENERATOR_ID_TREE;
}
