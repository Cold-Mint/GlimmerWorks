//
// Created by coldmint on 2026/3/29.
//

#include "BooleanToggleDynamicSuggestions.h"

#include "core/Constants.h"

std::string glimmer::BooleanToggleDynamicSuggestions::GetId() const {
    return BOOL_TOGGLE_DYNAMIC_SUGGESTIONS_NAME;
}

std::vector<std::string> glimmer::BooleanToggleDynamicSuggestions::GetSuggestions(std::string param) {
    return {"true", "false",TOGGLE_KEY_WORD};
}

bool glimmer::BooleanToggleDynamicSuggestions::Match(const std::string keyword, std::string param) {
    return keyword == "true" || keyword == "false" || keyword == "no" || keyword == "n" || keyword == "yes" || keyword
        == "y" || keyword == "0" || keyword == "1" || keyword == TOGGLE_KEY_WORD;
}
