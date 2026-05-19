//
// Created by Cold-Mint on 2026/2/22.
//

#pragma once
#include <memory>

#include "core/world/structure/IStructureConditionProcessor.h"

namespace glimmer {
    class StructurePlacementConditionsManager {
        std::unordered_map<std::string, std::unique_ptr<IStructureConditionProcessor> > conditionProcessors_;

    public:
        void AddConditionProcessor(std::unique_ptr<IStructureConditionProcessor> structureConditionProcessor);

        IStructureConditionProcessor *FindConditionProcessors(const std::string &id);
    };
}
