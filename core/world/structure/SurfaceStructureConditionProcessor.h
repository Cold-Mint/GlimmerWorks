//
// Created by coldmint on 2026/2/22.
//

#ifndef GLIMMERWORKS_SURFACEPLACMENTCONDITIONS_H
#define GLIMMERWORKS_SURFACEPLACMENTCONDITIONS_H
#include "IStructureConditionProcessor.h"

namespace glimmer {
    class SurfaceStructureConditionProcessor : public IStructureConditionProcessor {
    public:
        std::string GetName() override;

        std::bitset<CHUNK_AREA>
        Match(TerrainResult *terrainResult, const VariableConfig &variableConfig) override;
    };
}

#endif //GLIMMERWORKS_SURFACEPLACMENTCONDITIONS_H
