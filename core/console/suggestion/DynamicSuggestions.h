//
// Created by Cold-Mint on 2025/11/27.
//

#pragma once
#include <string>
#include <vector>

namespace glimmer {
    class DynamicSuggestions {
    public:
        virtual ~DynamicSuggestions();

        [[nodiscard]] virtual std::string GetId() const = 0;

        virtual std::vector<std::string> GetSuggestions(std::string param) = 0;

        virtual bool Match(std::string keyword, std::string param) = 0;
    };
}
