//
// Created by coldmint on 2025/12/23.
//

#ifndef GLIMMERWORKS_COMPOSABLEITEM_H
#define GLIMMERWORKS_COMPOSABLEITEM_H
#include <vector>

#include "Item.h"
#include "ItemFunctionMod.h"

namespace glimmer {
    /**
     * ComposableItem
     * 可组合的物品
     */
    class ComposableItem : public Item {
        std::pmr::vector<std::unique_ptr<ItemFunctionMod> > itemFunctionMods_;

    public:
        [[nodiscard]] std::string GetId() const override;

        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] std::string GetDescription() const override;

        [[nodiscard]] std::shared_ptr<SDL_Texture> GetIcon() const override;

        void AddFunctionMod(std::unique_ptr<ItemFunctionMod> itemFunctionMod);

        void RemoveFunctionMod(const ItemFunctionMod *mod);

        void OnUse() override;

        void OnDrop() override;
    };
}

#endif //GLIMMERWORKS_COMPOSABLEITEM_H
