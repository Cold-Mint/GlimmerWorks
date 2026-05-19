//
// Created by Cold-Mint on 2026/2/22.
//

#pragma once
#include <string>
#include <bitset>

#include "core/world/generator/TerrainResult.h"

namespace glimmer {
    class IStructureConditionProcessor {
    public:
        virtual ~IStructureConditionProcessor() = default;

        virtual std::string GetName() = 0;

        virtual std::bitset<CHUNK_AREA> Match(TerrainResult *terrainResult,
                                              const VariableConfig &variableConfig) = 0;
    };
}
