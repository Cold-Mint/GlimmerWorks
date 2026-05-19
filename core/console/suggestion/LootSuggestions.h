//
// Created by Cold-Mint on 2026/2/1.
//

#pragma once
#include "DynamicSuggestions.h"
#include "core/lootTable/LootTableManager.h"

namespace glimmer {
    class LootSuggestions final : public DynamicSuggestions {
        LootTableManager *lootTableManager_;

    public:
        explicit LootSuggestions(LootTableManager *lootTableManager);

        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;

        bool Match(std::string keyword, std::string param) override;
    };
}
