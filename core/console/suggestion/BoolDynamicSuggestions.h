//
// Created by Cold-Mint on 2025/11/27.
//

#ifndef GLIMMERWORKS_BOOLDYNAMICSUGGESTIONS_H
#define GLIMMERWORKS_BOOLDYNAMICSUGGESTIONS_H
#include "DynamicSuggestions.h"

namespace glimmer {
    class BoolDynamicSuggestions final : public DynamicSuggestions {
    public:
        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions() override;
    };
}

#endif //GLIMMERWORKS_BOOLDYNAMICSUGGESTIONS_H
