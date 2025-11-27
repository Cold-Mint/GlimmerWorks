//
// Created by Cold-Mint on 2025/11/27.
//

#ifndef GLIMMERWORKS_DYNAMICSUGGESTIONSMANAGER_H
#define GLIMMERWORKS_DYNAMICSUGGESTIONSMANAGER_H
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

#endif //GLIMMERWORKS_DYNAMICSUGGESTIONSMANAGER_H
