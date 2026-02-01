//
// Created by coldmint on 2026/2/1.
//

#include "LootSuggestions.h"

#include "core/Constants.h"

std::string glimmer::LootSuggestions::GetId() const {
    return LOOT_DYNAMIC_SUGGESTIONS_NAME;
}

std::vector<std::string> glimmer::LootSuggestions::GetSuggestions(std::string param) {
    return lootTableManager_->GetLootTableList();
}

bool glimmer::LootSuggestions::Match(std::string keyword, std::string param) {
    for (const auto &itemId: lootTableManager_->GetLootTableList()) {
        if (itemId == keyword) {
            return true;
        }
    }
    return false;
}
