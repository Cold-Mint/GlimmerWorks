//
// Created by coldmint on 2026/2/7.
//

#include "TreeStructureGenerator.h"


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
    int trunkWidth = 2;
    const VariableDefinition *trunkWidthDefinition = structureResource->generatorConfig.FindVariable("trunkWidth");
    if (trunkWidthDefinition != nullptr) {
        trunkWidth = trunkWidthDefinition->AsInt();
    }
    ResourceRef &trunkRef = structureResource->data.at(trunkDataIndex);
    ResourceRef &leafRef = structureResource->data.at(leafDataIndex);
    auto structureInfo = StructureInfo(startPosition);
    //TODO：放置树木

    //设置树干
    structureInfo.SetTile(0,0,trunkRef);
    //设置树叶
    structureInfo.SetTile(1,1,leafRef);
    return structureInfo;
}

std::string glimmer::TreeStructureGenerator::GetStructureGeneratorId() {
    return STRUCTURE_GENERATOR_ID_TREE;
}
