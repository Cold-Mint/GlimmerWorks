//
// Created by coldmint on 2025/12/23.
//

#include "ComposableItemDynamicSuggestions.h"

#include "../../Constants.h"

std::string glimmer::ComposableItemDynamicSuggestions::GetId() const {
    return COMPOSABLE_ITEM_DYNAMIC_SUGGESTIONS_NAME;
}

std::vector<std::string> glimmer::ComposableItemDynamicSuggestions::GetSuggestions(const std::string param) {
    return itemManager_->GetItemIDList();
}

bool glimmer::ComposableItemDynamicSuggestions::Match(const std::string keyword, std::string param) {
    for (const auto &itemId: itemManager_->GetItemIDList()) {
        if (itemId == keyword) {
            return true;
        }
    }
    return false;
}
