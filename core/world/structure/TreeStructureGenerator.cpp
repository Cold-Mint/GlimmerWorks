//
// Created by coldmint on 2026/2/7.
//

#include "TreeStructureGenerator.h"

#include <random>


void glimmer::TreeStructureGenerator::SetWorldSeed(int worldSeed) {
    IStructureGenerator::SetWorldSeed(worldSeed);
}

glimmer::StructureInfo glimmer::TreeStructureGenerator::Generate(TileVector2D startPosition,
                                                                 StructureResource *structureResource) {
    int leafDataIndex = 0;
    const VariableDefinition *leafDataIndexDefinition = structureResource->generatorConfig.
            FindVariable("leafDataIndex");
    if (leafDataIndexDefinition != nullptr) {
        leafDataIndex = leafDataIndexDefinition->AsInt();
    }
    int trunkDataIndex = 1;
    const VariableDefinition *trunkDataIndexDefinition = structureResource->generatorConfig.FindVariable(
        "trunkDataIndex");
    if (trunkDataIndexDefinition != nullptr) {
        trunkDataIndex = trunkDataIndexDefinition->AsInt();
    }
    int trunkHeightMax = 9;
    const VariableDefinition *trunkHeightMaxDefinition = structureResource->generatorConfig.FindVariable(
        "trunkHeightMax");
    if (trunkHeightMaxDefinition != nullptr) {
        trunkHeightMax = trunkHeightMaxDefinition->AsInt();
    }
    int trunkHeightMin = 3;
    const VariableDefinition *trunkHeightMinDefinition = structureResource->generatorConfig.FindVariable(
        "trunkHeightMin");
    if (trunkHeightMinDefinition != nullptr) {
        trunkHeightMin = trunkHeightMinDefinition->AsInt();
    }
    bool hasLeaves = false;
    const VariableDefinition *hasLeavesDefinition = structureResource->generatorConfig.FindVariable("hasLeaves");
    if (hasLeavesDefinition != nullptr) {
        hasLeaves = hasLeavesDefinition->AsBool();
    }
    int leafRadius = 3;
    const VariableDefinition *leafRadiusDefinition = structureResource->generatorConfig.FindVariable("leafRadius");
    if (leafRadiusDefinition != nullptr) {
        leafRadius = leafRadiusDefinition->AsInt();
    }
    int leafClusterCount = 1;
    const VariableDefinition *leafClusterCountDefinition = structureResource->generatorConfig.FindVariable(
        "leafClusterCount");
    if (leafClusterCountDefinition != nullptr) {
        leafClusterCount = leafClusterCountDefinition->AsInt();
    }
    int leafVerticalSpacing = 1;
    const VariableDefinition *leafVerticalSpacingDefinition = structureResource->generatorConfig.FindVariable(
        "leafVerticalSpacing");
    if (leafVerticalSpacingDefinition != nullptr) {
        leafVerticalSpacing = leafVerticalSpacingDefinition->AsInt();
    }
    int trunkWidth = 1;
    const VariableDefinition *trunkWidthDefinition = structureResource->generatorConfig.FindVariable("trunkWidth");
    if (trunkWidthDefinition != nullptr) {
        trunkWidth = trunkWidthDefinition->AsInt();
    }
    ResourceRef &trunkRef = structureResource->data.at(trunkDataIndex);
    ResourceRef &leafRef = structureResource->data.at(leafDataIndex);
    auto structureInfo = StructureInfo(startPosition);
    std::mt19937 rng(worldSeed_ ^
                     startPosition.x * 73428767 ^
                     startPosition.y * 912931);

    std::uniform_int_distribution heightDist(trunkHeightMin, trunkHeightMax);
    int trunkHeight = heightDist(rng);

    // 树干
    for (int y = 0; y < trunkHeight; ++y) {
        for (int x = 0; x < trunkWidth; ++x) {
            structureInfo.SetTile(x, -y, trunkRef);
        }
    }
    if (hasLeaves) {
        // 树叶
        for (int i = 0; i < leafClusterCount; ++i) {
            int clusterY = -trunkHeight - i * leafVerticalSpacing;

            for (int x = -leafRadius; x <= leafRadius; ++x) {
                for (int y = -leafRadius; y <= leafRadius; ++y) {
                    if (x * x + y * y <= leafRadius * leafRadius && y >= 0) {
                        structureInfo.SetTile(
                            x + trunkWidth / 2,
                            clusterY - y,
                            leafRef
                        );
                    }
                }
            }
        }
    }
    return structureInfo;
}

std::string glimmer::TreeStructureGenerator::GetStructureGeneratorId() {
    return STRUCTURE_GENERATOR_ID_TREE;
}
