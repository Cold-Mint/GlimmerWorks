//
// Created by coldmint on 2026/4/6.
//

#pragma once
#include "DynamicSuggestions.h"
#include "core/mod/dataPack/BiomesManager.h"


namespace glimmer {
    class BiomeSuggestions final : public DynamicSuggestions {
        BiomesManager *biomesManager_;

    public:
        explicit BiomeSuggestions(BiomesManager *biomesManager);

        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;

        bool Match(std::string keyword, std::string param) override;
    };
}
