//
// Created by coldmint on 2025/12/23.
//

#include "ComposableItem.h"

std::string glimmer::ComposableItem::GetId() const {
}

std::string glimmer::ComposableItem::GetName() const {
}

std::string glimmer::ComposableItem::GetDescription() const {
}

std::shared_ptr<SDL_Texture> glimmer::ComposableItem::GetIcon() const {
}

void glimmer::ComposableItem::AddFunctionMod(std::unique_ptr<ItemFunctionMod> itemFunctionMod) {
    itemFunctionMods_.emplace_back(std::move(itemFunctionMod));
}

void glimmer::ComposableItem::RemoveFunctionMod(const ItemFunctionMod *mod) {
    const auto it = std::remove_if(
        itemFunctionMods_.begin(),
        itemFunctionMods_.end(),
        [&](const std::unique_ptr<ItemFunctionMod> &ptr) {
            return ptr.get() == mod;
        }
    );

    itemFunctionMods_.erase(it, itemFunctionMods_.end());
}

void glimmer::ComposableItem::OnUse() {
}

void glimmer::ComposableItem::OnDrop() {
}
