//
// Created by coldmint on 2026/5/3.
//

#include "MouseButtonDynamicSuggestions.h"

#include "core/Constants.h"
#include "core/utils/MouseButtonUtils.h"

std::string glimmer::MouseButtonDynamicSuggestions::GetId() const {
    return MOUSE_BUTTON_DYNAMIC_SUGGESTIONS_NAME;
}

std::vector<std::string> glimmer::MouseButtonDynamicSuggestions::GetSuggestions(std::string param) {
    return MouseButtonUtils::GetAllMouseButtonKeys();
}

bool glimmer::MouseButtonDynamicSuggestions::Match(const std::string keyword, std::string param) {
    return MouseButtonUtils::ContainsKey(keyword);
}
