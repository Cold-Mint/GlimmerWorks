//
// Created by Cold-Mint on 2026/2/22.
//

#ifndef GLIMMERWORKS_ISTRUCTURECONDITIONPROCESSOR_H
#define GLIMMERWORKS_ISTRUCTURECONDITIONPROCESSOR_H
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

#endif //GLIMMERWORKS_ISTRUCTURECONDITIONPROCESSOR_H
