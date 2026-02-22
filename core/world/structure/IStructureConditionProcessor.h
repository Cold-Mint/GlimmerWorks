//
// Created by coldmint on 2026/2/22.
//

#ifndef GLIMMERWORKS_STRUCTUREPLACEMENTCONDITIONS_H
#define GLIMMERWORKS_STRUCTUREPLACEMENTCONDITIONS_H
#include <string>

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

#endif //GLIMMERWORKS_STRUCTUREPLACEMENTCONDITIONS_H
