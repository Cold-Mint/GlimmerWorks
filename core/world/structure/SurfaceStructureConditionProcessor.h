//
// Created by Cold-Mint on 2026/2/22.
//

#pragma once
#include "IStructureConditionProcessor.h"

namespace glimmer {
    class SurfaceStructureConditionProcessor : public IStructureConditionProcessor {
    public:
        std::string GetName() override;

        std::bitset<CHUNK_AREA>
        Match(TerrainResult *terrainResult, const VariableConfig &variableConfig) override;
    };
}
