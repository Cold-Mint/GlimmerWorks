//
// Created by coldmint on 2025/12/28.
//

#ifndef GLIMMERWORKS_ABILITYITEMDYNAMICSUGGESTIONS_H
#define GLIMMERWORKS_ABILITYITEMDYNAMICSUGGESTIONS_H
#include "DynamicSuggestions.h"
#include "../../mod/dataPack/ItemManager.h"

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


#endif //GLIMMERWORKS_ABILITYITEMDYNAMICSUGGESTIONS_H
