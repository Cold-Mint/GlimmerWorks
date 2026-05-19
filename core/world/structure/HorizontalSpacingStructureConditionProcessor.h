//
// Created by Cold-Mint on 2026/2/23.
//

#pragma once
#include "IStructureConditionProcessor.h"

namespace glimmer {
    class HorizontalSpacingStructureConditionProcessor : public IStructureConditionProcessor {
    public:
        std::string GetName() override;

        std::bitset<CHUNK_AREA> Match(TerrainResult *terrainResult, const VariableConfig &variableConfig) override;
    };
}
