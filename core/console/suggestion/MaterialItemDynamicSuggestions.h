//
// Created by coldmint on 2026/5/27.
//

#pragma once
#include "DynamicSuggestions.h"
#include "core/mod/dataPack/ItemManager.h"

namespace glimmer
{
    class MaterialItemDynamicSuggestions final : public DynamicSuggestions
    {
        ItemManager* itemManager_;

    public:
        explicit MaterialItemDynamicSuggestions(ItemManager* itemManager);

        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;

        bool Match(std::string keyword, std::string param) override;
    };
}
