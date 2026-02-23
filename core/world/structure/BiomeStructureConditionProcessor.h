//
// Created by coldmint on 2026/2/22.
//

#ifndef GLIMMERWORKS_BIOMESTRUCTURECONDITIONPROCESSOR_H
#define GLIMMERWORKS_BIOMESTRUCTURECONDITIONPROCESSOR_H
#include "IStructureConditionProcessor.h"

namespace glimmer {
    class BiomeStructureConditionProcessor : public IStructureConditionProcessor {
    public:
        std::string GetName() override;

        std::bitset<CHUNK_AREA>
        Match(TerrainResult *terrainResult, const VariableConfig &variableConfig) override;
    };
}


#endif //GLIMMERWORKS_BIOMESTRUCTURECONDITIONPROCESSOR_H
