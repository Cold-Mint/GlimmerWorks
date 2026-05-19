//
// Created by Cold-Mint on 2025/12/28.
//

#pragma once
#include "DynamicSuggestions.h"
#include "core/mod/dataPack/ItemManager.h"

namespace glimmer {
    class AbilityItemDynamicSuggestions final : public DynamicSuggestions {
        ItemManager *itemManager_;

    public:
        explicit AbilityItemDynamicSuggestions(ItemManager *itemManager);

        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;

        bool Match(std::string keyword, std::string param) override;
    };
}
