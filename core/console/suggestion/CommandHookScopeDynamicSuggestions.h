//
// Created by coldmint on 2026/5/2.
//

#ifndef GLIMMERWORKS_COMMANDHOOKSCDYNAMICSUGGESTIONS_H
#define GLIMMERWORKS_COMMANDHOOKSCDYNAMICSUGGESTIONS_H
#include "DynamicSuggestions.h"

namespace glimmer {
    class CommandHookScopeDynamicSuggestions final : public DynamicSuggestions {
    public:
        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;

        bool Match(std::string keyword, std::string param) override;
    };
}


#endif //GLIMMERWORKS_COMMANDHOOKSCDYNAMICSUGGESTIONS_H
