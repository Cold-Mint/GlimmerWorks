//
// Created by coldmint on 2026/2/22.
//

#include "StructurePlacementConditionsManager.h"

void glimmer::StructurePlacementConditionsManager::AddConditionProcessor(
    std::unique_ptr<IStructureConditionProcessor> structureConditionProcessor) {
    std::string id = structureConditionProcessor->GetName();
    conditionProcessors_.emplace(id, std::move(structureConditionProcessor));
}

glimmer::IStructureConditionProcessor *glimmer::StructurePlacementConditionsManager::
FindConditionProcessors(const std::string &id) {
    const auto it = conditionProcessors_.find(id);
    if (it != conditionProcessors_.end()) {
        return it->second.get();
    }
    return nullptr;
}
