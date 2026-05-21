//
// Created by Cold-Mint on 2026/2/7.
//

#include "TreeStructureGenerator.h"

#include <random>


void glimmer::TreeStructureGenerator::SetWorldSeed(int worldSeed) {
    IStructureGenerator::SetWorldSeed(worldSeed);
}

std::optional<glimmer::StructureInfo> glimmer::TreeStructureGenerator::Generate(TileVector2D startPosition,
    IStructureResource *structureResource) {
    if (structureResource == nullptr) {
        return std::nullopt;
    }
    auto *treeStructureResource = dynamic_cast<TreeStructureResource *>(structureResource);
    ResourceRef &trunkRef = treeStructureResource->data.at(treeStructureResource->trunkDataIndex);
    ResourceRef &leafRef = treeStructureResource->data.at(treeStructureResource->leafDataIndex);
    auto structureInfo = StructureInfo();
    std::mt19937 rng(worldSeed_ ^
                     startPosition.x * 73428767 ^
                     startPosition.y * 912931);

    std::uniform_int_distribution heightDist(treeStructureResource->trunkHeightMin,
                                             treeStructureResource->trunkHeightMax);
    int trunkHeight = heightDist(rng);
    auto trunkTileLayer = static_cast<TileLayerType>(treeStructureResource->trunkTileLayer);
    for (int y = 0; y < trunkHeight; ++y) {
        for (int x = 0; x < treeStructureResource->trunkWidth; ++x) {
            structureInfo.SetTile(trunkTileLayer, TileVector2D(x, y), trunkRef);
        }
    }
    if (treeStructureResource->hasLeaves) {
        auto leafTileLayer = static_cast<TileLayerType>(treeStructureResource->leafTileLayer);
        u_int8_t leafRadius = treeStructureResource->leafRadius;
        for (int i = 0; i < treeStructureResource->leafClusterCount; ++i) {
            int clusterY = trunkHeight - i * treeStructureResource->leafVerticalSpacing;
            for (int x = -leafRadius; x <= leafRadius; ++x) {
                for (int y = -leafRadius; y <= leafRadius; ++y) {
                    if (x * x + y * y <= leafRadius * leafRadius && y >= 0) {
                        structureInfo.SetTile(leafTileLayer,
                                              TileVector2D{
                                                  x + treeStructureResource->trunkWidth / 2,
                                                  clusterY + y
                                              },
                                              leafRef
                        );
                    }
                }
            }
        }
    }
    return structureInfo;
}


glimmer::StructureGeneratorType glimmer::TreeStructureGenerator::GetStructureGeneratorType() const {
    return StructureGeneratorType::Tree;
}
