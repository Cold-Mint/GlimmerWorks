//
// Created by coldmint on 2026/2/7.
//

#include "StaticStructureGenerator.h"

glimmer::StructureInfo glimmer::StaticStructureGenerator::Generate(TileVector2D structuralOrigin,
                                                                   StructureResource *structureResource) {
    auto structureInfo = StructureInfo();
    structureInfo.SetTileData(structureResource->width, structureResource->data);
    return structureInfo;
}

std::string glimmer::StaticStructureGenerator::GetStructureGeneratorId() {
    return STRUCTURE_GENERATOR_ID_STATIC;
}
