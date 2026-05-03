//
// Created by coldmint on 2026/5/3.
//

#include "EventTypeDynamicSuggestions.h"

#include "core/Constants.h"
#include "core/utils/EventTypeUtils.h"

std::string glimmer::EventTypeDynamicSuggestions::GetId() const {
    return EVENT_TYPE_DYNAMIC_SUGGESTIONS_NAME;
}

std::vector<std::string> glimmer::EventTypeDynamicSuggestions::GetSuggestions(std::string param) {
    return EventTypeUtils::GetAllEventTypeKeys();
}

bool glimmer::EventTypeDynamicSuggestions::Match(const std::string keyword, std::string param) {
    return EventTypeUtils::ContainsKey(keyword);
}
