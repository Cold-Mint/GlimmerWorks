//
// Created by coldmint on 2025/12/28.
//

#include "AbilityItemDynamicSuggestions.h"

#include "../../Constants.h"

std::string glimmer::AbilityItemDynamicSuggestions::GetId() const {
    return ABILITY_ITEM_DYNAMIC_SUGGESTIONS_NAME;
}

std::vector<std::string> glimmer::AbilityItemDynamicSuggestions::GetSuggestions(std::string param) {
    return itemManager_->GetAbilityItemIDList();
}

bool glimmer::AbilityItemDynamicSuggestions::Match(std::string keyword, std::string param) {
    for (const auto &itemId: itemManager_->GetAbilityItemIDList()) {
        if (itemId == keyword) {
            return true;
        }
    }
    return false;
}
