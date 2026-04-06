//
// Created by coldmint on 2026/4/6.
//

#include "BiomeSuggestions.h"

#include "core/Constants.h"

glimmer::BiomeSuggestions::BiomeSuggestions(BiomesManager *biomesManager) : biomesManager_(biomesManager) {
}

std::string glimmer::BiomeSuggestions::GetId() const {
    return BIOME_DYNAMIC_SUGGESTIONS_NAME;
}

std::vector<std::string> glimmer::BiomeSuggestions::GetSuggestions(std::string param) {
    return biomesManager_->GetBiomeList();
}

bool glimmer::BiomeSuggestions::Match(const std::string keyword, std::string param) {
    for (const auto &biomeId: biomesManager_->GetBiomeList()) {
        if (biomeId == keyword) {
            return true;
        }
    }
    return false;
}
