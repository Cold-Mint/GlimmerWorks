//
// Created by coldmint on 2026/5/2.
//

#include "CommandHookIdDynamicSuggestions.h"

#include "core/Constants.h"

glimmer::CommandHookIdDynamicSuggestions::CommandHookIdDynamicSuggestions(
    CommandHookManager *commandHookManager) : commandHookManager_(commandHookManager) {
}

std::string glimmer::CommandHookIdDynamicSuggestions::GetId() const {
    return COMMAND_HOOK_ID_SUGGESTION_NAME;
}

std::vector<std::string> glimmer::CommandHookIdDynamicSuggestions::GetSuggestions(std::string param) {
    return commandHookManager_->GetCommandHookIdsWithOutConfig();
}

bool glimmer::CommandHookIdDynamicSuggestions::Match(const std::string keyword, std::string param) {
    auto array = commandHookManager_->GetCommandHookIdsWithOutConfig();
    for (auto &basicString: array) {
        if (basicString == keyword) {
            return true;
        }
    }
    return false;
}
