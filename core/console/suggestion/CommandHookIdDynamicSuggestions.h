//
// Created by coldmint on 2026/5/2.
//

#ifndef GLIMMERWORKS_COMMANDHOOKIDDYNAMICSUGGESTIONS_H
#define GLIMMERWORKS_COMMANDHOOKIDDYNAMICSUGGESTIONS_H
#include "DynamicSuggestions.h"
#include "core/CommandHookManager.h"

namespace glimmer {
    class CommandHookIdDynamicSuggestions final : public DynamicSuggestions {
        CommandHookManager *commandHookManager_ = nullptr;

    public:
        explicit CommandHookIdDynamicSuggestions(CommandHookManager *commandHookManager);

        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;

        bool Match(std::string keyword, std::string param) override;
    };
}

#endif //GLIMMERWORKS_COMMANDHOOKIDDYNAMICSUGGESTIONS_H
