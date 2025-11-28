//
// Created by Cold-Mint on 2025/11/27.
//

#ifndef GLIMMERWORKS_DYNAMICSUGGESTIONS_H
#define GLIMMERWORKS_DYNAMICSUGGESTIONS_H
#include <string>
#include <vector>

namespace glimmer {
    class DynamicSuggestions {
    public:
        virtual ~DynamicSuggestions();

        [[nodiscard]] virtual std::string GetId() const = 0;

        virtual std::vector<std::string> GetSuggestions(std::string param) = 0;
    };
}

#endif //GLIMMERWORKS_DYNAMICSUGGESTIONS_H
