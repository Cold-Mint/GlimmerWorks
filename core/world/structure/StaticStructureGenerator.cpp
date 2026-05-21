//
// Created by Cold-Mint on 2026/2/7.
//

#include "StaticStructureGenerator.h"


std::optional<glimmer::StructureInfo> glimmer::StaticStructureGenerator::Generate(TileVector2D startPosition,
    IStructureResource *structureResource) {
    if (structureResource == nullptr) {
        return std::nullopt;
    }
    StructureInfo structureInfo;
    auto staticStructureResource = dynamic_cast<StaticStructureResource *>(structureResource);
    for (auto &tileInfo: staticStructureResource->tileInfo) {
        structureInfo.SetTile(static_cast<TileLayerType>(tileInfo.layerType),
                              {tileInfo.position.x, tileInfo.position.y}, tileInfo.tile);
    }
    return structureInfo;
}

glimmer::StructureGeneratorType glimmer::StaticStructureGenerator::GetStructureGeneratorType() const {
    return StructureGeneratorType::Static;
}
