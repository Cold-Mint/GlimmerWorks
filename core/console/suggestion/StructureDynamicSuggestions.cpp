//
// Created by coldmint on 2026/2/13.
//

#include "StructureDynamicSuggestions.h"

glimmer::StructureDynamicSuggestions::StructureDynamicSuggestions(
    StructureManager *structureManager) : structureManager_(structureManager) {
}

std::string glimmer::StructureDynamicSuggestions::GetId() const {
    return STRUCTURE_DYNAMIC_SUGGESTIONS_NAME;
}

std::vector<std::string> glimmer::StructureDynamicSuggestions::GetSuggestions(std::string param) {
    return structureManager_->GetStructureIDList();
}

bool glimmer::StructureDynamicSuggestions::Match(std::string keyword, std::string param) {
    for (const auto &itemId: structureManager_->GetStructureIDList()) {
        if (itemId == keyword) {
            return true;
        }
    }
    return false;
}
