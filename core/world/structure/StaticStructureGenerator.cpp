//
// Created by coldmint on 2026/2/7.
//

#include "StaticStructureGenerator.h"

void glimmer::StaticStructureGenerator::SetWidth(const uint32_t width) {
    width_ = width;
}

void glimmer::StaticStructureGenerator::SetTileData(const std::vector<ResourceRef> &tileData) {
    tileData_ = tileData;
}

glimmer::StructureInfo glimmer::StaticStructureGenerator::Generate(const TileVector2D startPosition) {
    return StructureInfo(width_, tileData_, startPosition);
}

std::string glimmer::StaticStructureGenerator::GetStructureGeneratorId() {
    return STRUCTURE_GENERATOR_ID_STATIC;
}
