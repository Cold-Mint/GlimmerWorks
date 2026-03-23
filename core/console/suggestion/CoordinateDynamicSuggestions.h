//
// Created by Cold-Mint on 2026/3/3.
//

#ifndef GLIMMERWORKS_COORDINATEYDYNAMICSUGGESTIONS_H
#define GLIMMERWORKS_COORDINATEYDYNAMICSUGGESTIONS_H
#include "DynamicSuggestions.h"

namespace glimmer {
    class CoordinateDynamicSuggestions final : public DynamicSuggestions {
        std::string id_;
    public:
        explicit CoordinateDynamicSuggestions(const std::string &id);

        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;

        bool Match(std::string keyword, std::string param) override;
    };
}


#endif //GLIMMERWORKS_COORDINATEYDYNAMICSUGGESTIONS_H
