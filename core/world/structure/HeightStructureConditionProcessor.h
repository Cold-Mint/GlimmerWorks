//
// Created by coldmint on 2026/2/22.
//

#ifndef GLIMMERWORKS_HEIGHTSTRUCTURECONDITIONPROCESSOR_H
#define GLIMMERWORKS_HEIGHTSTRUCTURECONDITIONPROCESSOR_H
#include "IStructureConditionProcessor.h"

namespace glimmer {
    class HeightStructureConditionProcessor : public IStructureConditionProcessor {
    public:
        std::string GetName() override;

        std::bitset<CHUNK_AREA> Match(TerrainResult *terrainResult,
                                                           const VariableConfig &variableConfig) override;
    };
}

#endif //GLIMMERWORKS_HEIGHTSTRUCTURECONDITIONPROCESSOR_H
