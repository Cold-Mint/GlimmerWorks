//
// Created by coldmint on 2025/12/23.
//

#ifndef GLIMMERWORKS_COMPOSABLEITEMDYNAMICSUGGESTIONS_H
#define GLIMMERWORKS_COMPOSABLEITEMDYNAMICSUGGESTIONS_H
#include "DynamicSuggestions.h"
#include "../../mod/dataPack/ItemManager.h"

namespace glimmer {
    class ComposableItemDynamicSuggestions final : public DynamicSuggestions {
        ItemManager *itemManager_;

    public:
        explicit ComposableItemDynamicSuggestions(ItemManager *itemManager);

        [[nodiscard]] std::string GetId() const override;

        std::vector<std::string> GetSuggestions(std::string param) override;

        bool Match(std::string keyword, std::string param) override;
    };
}

#endif //GLIMMERWORKS_COMPOSABLEITEMDYNAMICSUGGESTIONS_H
