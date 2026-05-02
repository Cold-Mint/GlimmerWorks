//
// Created by coldmint on 2026/5/2.
//

#include "CommandHookScopeDynamicSuggestions.h"

#include "core/Constants.h"

std::string glimmer::CommandHookScopeDynamicSuggestions::GetId() const {
    return COMMAND_HOOK_SCOPE_DYNAMIC_SUGGESTIONS_NAME;
}

std::vector<std::string> glimmer::CommandHookScopeDynamicSuggestions::GetSuggestions(std::string param) {
    return {SESSION_KEY_WORD};
}

bool glimmer::CommandHookScopeDynamicSuggestions::Match(const std::string keyword, std::string param) {
    return keyword == SESSION_KEY_WORD;
}
