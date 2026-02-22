//
// Created by coldmint on 2026/2/22.
//

#ifndef GLIMMERWORKS_STRUCTUREPLACEMENTCONDITIONSMANAGER_H
#define GLIMMERWORKS_STRUCTUREPLACEMENTCONDITIONSMANAGER_H
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


#endif //GLIMMERWORKS_STRUCTUREPLACEMENTCONDITIONSMANAGER_H
