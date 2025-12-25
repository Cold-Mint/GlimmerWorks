//
// Created by coldmint on 2025/12/23.
//

#include "ComposableItem.h"

std::string glimmer::ComposableItem::GetId() const {
    return id_;
}

std::string glimmer::ComposableItem::GetName() const {
    return name_;
}

std::string glimmer::ComposableItem::GetDescription() const {
    return description_;
}

std::shared_ptr<SDL_Texture> glimmer::ComposableItem::GetIcon() const {
    return icon_;
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

void glimmer::ComposableItem::OnUse(AppContext *appContext, WorldContext *worldContext, GameEntity *user) {
    for (const auto &mod: itemFunctionMods_) {
        mod->OnUse(appContext, worldContext, user);
    }
}

void glimmer::ComposableItem::OnDrop() {
}

size_t glimmer::ComposableItem::GetMaxSlotSize() const {
    return maxSlotSize_;
}

const std::pmr::vector<std::unique_ptr<glimmer::ItemFunctionMod> > &glimmer::ComposableItem::GetModules() const {
    return itemFunctionMods_;
}
