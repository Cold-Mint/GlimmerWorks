//
// Created by coldmint on 2026/3/29.
//

#ifndef GLIMMERWORKS_BOOLEANTOGGLEDYNAMICSUGGESTIONS_H
#define GLIMMERWORKS_BOOLEANTOGGLEDYNAMICSUGGESTIONS_H
#include "DynamicSuggestions.h"


namespace glimmer {
    class BooleanToggleDynamicSuggestions final : public DynamicSuggestions {
    public:
        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;

        bool Match(std::string keyword, std::string param) override;
    };
}


#endif //GLIMMERWORKS_BOOLEANTOGGLEDYNAMICSUGGESTIONS_H
