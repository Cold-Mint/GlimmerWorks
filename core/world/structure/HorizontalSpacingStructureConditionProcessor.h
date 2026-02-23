//
// Created by coldmint on 2026/2/23.
//

#ifndef GLIMMERWORKS_HORIZONTALSPACINGSTRUCTURECONDITIONPROCESSOR_H
#define GLIMMERWORKS_HORIZONTALSPACINGSTRUCTURECONDITIONPROCESSOR_H
#include "IStructureConditionProcessor.h"

namespace glimmer {
    class HorizontalSpacingStructureConditionProcessor : public IStructureConditionProcessor {
    public:
        std::string GetName() override;

        std::bitset<CHUNK_AREA> Match(TerrainResult *terrainResult, const VariableConfig &variableConfig) override;
    };
}

#endif //GLIMMERWORKS_HORIZONTALSPACINGSTRUCTURECONDITIONPROCESSOR_H
