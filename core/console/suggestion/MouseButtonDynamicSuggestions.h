//
// Created by coldmint on 2026/5/3.
//

#ifndef GLIMMERWORKS_MUSEBUTTONDYNAMICSUGGESTIONS_H
#define GLIMMERWORKS_MUSEBUTTONDYNAMICSUGGESTIONS_H
#include "DynamicSuggestions.h"

namespace glimmer {
    class MouseButtonDynamicSuggestions final : public DynamicSuggestions {
    public:
        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;

        bool Match(std::string keyword, std::string param) override;
    };
}

#endif //GLIMMERWORKS_MUSEBUTTONDYNAMICSUGGESTIONS_H
