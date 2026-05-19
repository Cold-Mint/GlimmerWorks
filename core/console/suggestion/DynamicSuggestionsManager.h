//
// Created by Cold-Mint on 2025/11/27.
//

#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "DynamicSuggestions.h"

namespace glimmer {
    class DynamicSuggestionsManager {
        std::unordered_map<std::string, std::unique_ptr<DynamicSuggestions> > dynamicSuggestions_{};

    public:
        void RegisterDynamicSuggestions(std::unique_ptr<DynamicSuggestions> dynamicSuggestion);

        [[nodiscard]] DynamicSuggestions *GetSuggestions(const std::string &id) const;
    };
}
