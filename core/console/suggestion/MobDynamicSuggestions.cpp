//
// Created by Cold-Mint on 2026/3/3.
//

#include "MobDynamicSuggestions.h"

#include "core/Constants.h"

glimmer::MobDynamicSuggestions::MobDynamicSuggestions(MobManager *mobManager) : mobManager_(mobManager) {
}

std::string glimmer::MobDynamicSuggestions::GetId() const {
    return MOB_DYNAMIC_SUGGESTIONS_NAME;
}

std::vector<std::string> glimmer::MobDynamicSuggestions::GetSuggestions(std::string param) {
    return mobManager_->GetMobList();
}

bool glimmer::MobDynamicSuggestions::Match(std::string keyword, std::string param) {
    for (const auto &mobId: mobManager_->GetMobList()) {
        if (mobId == keyword) {
            return true;
        }
    }
    return false;
}
