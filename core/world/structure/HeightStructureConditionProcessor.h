//
// Created by coldmint on 2026/2/22.
//

#ifndef GLIMMERWORKS_HEIGHTPLACEMENTCONDITIONS_H
#define GLIMMERWORKS_HEIGHTPLACEMENTCONDITIONS_H
#include "IStructureConditionProcessor.h"

namespace glimmer {
    class HeightStructureConditionProcessor : public IStructureConditionProcessor {
    public:
        std::string GetName() override;

        std::bitset<CHUNK_AREA> Match(TerrainResult *terrainResult,
                                                           const VariableConfig &variableConfig) override;
    };
}

#endif //GLIMMERWORKS_HEIGHTPLACEMENTCONDITIONS_H
