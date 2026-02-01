//
// Created by coldmint on 2026/2/1.
//

#ifndef GLIMMERWORKS_LOOTSUGGESTIONS_H
#define GLIMMERWORKS_LOOTSUGGESTIONS_H
#include "DynamicSuggestions.h"
#include "core/lootTable/LootTableManager.h"

namespace glimmer {
    class LootSuggestions final : public DynamicSuggestions {
        LootTableManager *lootTableManager_;

    public:
        explicit LootSuggestions(LootTableManager *loot_table_manager)
            : lootTableManager_(loot_table_manager) {
        }

        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;

        bool Match(std::string keyword, std::string param) override;
    };
}

#endif //GLIMMERWORKS_LOOTSUGGESTIONS_H
