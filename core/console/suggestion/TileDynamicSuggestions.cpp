//
// Created by coldmint on 2025/12/23.
//

#include "TileDynamicSuggestions.h"

#include "../../Constants.h"

std::string glimmer::TileDynamicSuggestions::GetId() const {
    return TILE_DYNAMIC_SUGGESTIONS_NAME;
}

std::vector<std::string> glimmer::TileDynamicSuggestions::GetSuggestions(std::string param) {
    return tileManager_->GetTileIDList();
}

bool glimmer::TileDynamicSuggestions::Match(std::string keyword, std::string param) {
    for (const auto &itemId: tileManager_->GetTileIDList()) {
        if (itemId == keyword) {
            return true;
        }
    }
    return false;
}
