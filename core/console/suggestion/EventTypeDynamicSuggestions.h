//
// Created by coldmint on 2026/5/3.
//

#ifndef GLIMMERWORKS_EVENTTYPEDYNAMICSUGGESTIONS_H
#define GLIMMERWORKS_EVENTTYPEDYNAMICSUGGESTIONS_H
#include "DynamicSuggestions.h"

namespace glimmer {
    class EventTypeDynamicSuggestions final : public DynamicSuggestions {
    public:
        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;

        bool Match(std::string keyword, std::string param) override;
    };
}

#endif //GLIMMERWORKS_EVENTTYPEDYNAMICSUGGESTIONS_H
