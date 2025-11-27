//
// Created by Cold-Mint on 2025/11/27.
//

#include "DynamicSuggestionsManager.h"

void glimmer::DynamicSuggestionsManager::RegisterDynamicSuggestions(
    std::unique_ptr<DynamicSuggestions> dynamicSuggestion) {
    const std::string id = dynamicSuggestion->GetId();
    dynamicSuggestions_[id] = std::move(dynamicSuggestion);
}

glimmer::DynamicSuggestions *glimmer::DynamicSuggestionsManager::GetSuggestions(const std::string &id) const {
    if (const auto it = dynamicSuggestions_.find(id); it != dynamicSuggestions_.end()) {
        return it->second.get();
    }
    return nullptr;
}
