//
// Created by coldmint on 2025/12/23.
//

#include "ComposableItem.h"

#include "AbilityItem.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/world/WorldContext.h"
#include <utility>
#include <vector>
#include "../log/LogCat.h"
#include "../../core/mod/ResourceLocator.h"
#include "ability/ItemAbility.h"


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

void glimmer::ComposableItem::SwapItem(size_t index, ItemContainer *otherContainer, size_t otherIndex) const {
    itemContainer_->SwapItem(index, otherContainer, otherIndex);
}

std::unique_ptr<glimmer::Item> glimmer::ComposableItem::ReplaceItem(const size_t index,
                                                                    std::unique_ptr<Item> item) const {
    return itemContainer_->ReplaceItem(index, std::move(item));
}

size_t glimmer::ComposableItem::RemoveItemAbility(const std::string &id, const size_t amount) const {
    return itemContainer_->RemoveItem(id, amount);
}

void glimmer::ComposableItem::OnUse(AppContext *appContext, WorldContext *worldContext, const GameEntity::ID user) {
    const size_t max = itemContainer_->GetCapacity();
    for (size_t index = 0; index < max; index++) {
        Item *item = itemContainer_->GetItem(index);
        if (item == nullptr) {
            continue;
        }
        auto *abilityItem = dynamic_cast<AbilityItem *>(item);
        if (abilityItem == nullptr) {
            //Throwing items that are not items of ability.
            //扔出不为能力物品的物品。
            worldContext->CreateDroppedItemEntity(
                itemContainer_->TakeAllItem(index),
                worldContext->GetCameraTransform2D()->GetPosition(),
                2
            );
            continue;
        }
        ItemAbility *itemAbility = abilityItem->GetItemAbility();
        if (itemAbility == nullptr) {
            //Throwing items that are not items of ability.
            //扔出不为能力物品的物品。
            worldContext->CreateDroppedItemEntity(
                itemContainer_->TakeAllItem(index),
                worldContext->GetCameraTransform2D()->GetPosition(),
                2
            );
            continue;
        }
        itemAbility->OnUse(appContext, worldContext, user);
    }
}

int glimmer::ComposableItem::TryParseItemIndex(const std::string &name) {
    constexpr std::string_view prefix = "item_";
    if (!name.starts_with(prefix)) {
        return -1;
    }
    std::string_view numberPart{name};
    numberPart.remove_prefix(prefix.size());
    if (numberPart.empty()) {
        return -1;
    }
    int value = -1;
    auto [ptr, ec] = std::from_chars(
        numberPart.data(),
        numberPart.data() + numberPart.size(),
        value
    );
    if (ec != std::errc{} || ptr != numberPart.data() + numberPart.size()) {
        return -1;
    }
    return value;
}

std::unique_ptr<glimmer::ComposableItem> glimmer::ComposableItem::FromItemResource(AppContext *appContext,
    const ComposableItemResource *itemResource, const ResourceRef &resourceRef) {
    std::string name = Resource::GenerateId(itemResource->packId, itemResource->key);
    const auto nameRes = appContext->GetResourceLocator()->FindString(itemResource->name);
    if (nameRes.has_value()) {
        name = nameRes.value()->value;
    }
    std::string description = Resource::GenerateId(itemResource->packId, itemResource->key);
    auto descriptionRes = appContext->GetResourceLocator()->FindString(itemResource->description);
    if (descriptionRes.has_value()) {
        description = descriptionRes.value()->value;
    }
    size_t slotSize = itemResource->slotSize;
    if (itemResource->missing) {
        size_t count = resourceRef.GetArgCount();
        slotSize = 0;
        for (int i = 0; i < count; i++) {
            auto refArg = resourceRef.GetArg(i);
            if (refArg.has_value()) {
                ResourceRefArg &arg = refArg.value();
                int index = TryParseItemIndex(arg.GetName());
                if (index < 0) {
                    continue;
                }
                slotSize = std::ranges::max(slotSize, static_cast<size_t>(index + 1));
            }
        }
    }
    auto result = std::make_unique<ComposableItem>(
        Resource::GenerateId(*itemResource), name,
        description,
        appContext->GetResourcePackManager()->LoadTextureFromFile(appContext, itemResource->texture),
        slotSize);
    //Filling ability.
    //填充能力。
    size_t argCount = resourceRef.GetArgCount();
    if (argCount == 0) {
        //If the capability is not specified within the resource reference, then the default capability will be loaded.
        //如果没有在资源引用内指定能力，那么加载默认能力。
        size_t defaultAbilitySize = itemResource->defaultAbilityList.size();
        for (int i = 0; i < defaultAbilitySize; i++) {
            auto itemObj = appContext->GetResourceLocator()->FindItem(
                appContext, itemResource->defaultAbilityList[i]);
            if (itemObj.has_value()) {
                (void) result->ReplaceItem(static_cast<size_t>(i), std::move(itemObj.value()));
            }
        }
    } else {
        for (int i = 0; i < argCount; i++) {
            auto refArg = resourceRef.GetArg(i);
            if (refArg.has_value()) {
                ResourceRefArg &arg = refArg.value();
                int index = TryParseItemIndex(arg.GetName());
                if (index < 0) {
                    continue;
                }
                auto itemRef = arg.AsResourceRef();
                if (!itemRef.has_value()) {
                    continue;
                }
                auto itemObj = appContext->GetResourceLocator()->FindItem(appContext, itemRef.value());
                if (itemObj.has_value()) {
                    (void) result->ReplaceItem(static_cast<size_t>(index), std::move(itemObj.value()));
                }
            }
        }
    }
    return result;
}

glimmer::ItemContainer *glimmer::ComposableItem::GetItemContainer() const {
    return itemContainer_.get();
}

std::unique_ptr<glimmer::Item> glimmer::ComposableItem::Clone() const {
    auto newItem = std::make_unique<ComposableItem>(id_, name_, description_, icon_, maxSlotSize_);
    newItem->itemContainer_ = itemContainer_->Clone();
    return newItem;
}

std::optional<glimmer::ResourceRef> glimmer::ComposableItem::ActualToResourceRef() {
    std::optional<ResourceRef> resourceRef = Resource::ParseFromId(id_, RESOURCE_TYPE_COMPOSABLE_ITEM);
    size_t size = itemContainer_->GetCapacity();
    for (int i = 0; i < size; i++) {
        Item *item = itemContainer_->GetItem(i);
        if (item == nullptr) {
            continue;
        }
        auto ref = item->ToResourceRef();
        if (ref.has_value()) {
            ResourceRefArg refArg;
            refArg.SetName("item_" + std::to_string(i));
            refArg.SetDataFromResourceRef(ref.value());
            resourceRef->AddArg(refArg);
        }
    }
    return resourceRef;
}

glimmer::ComposableItem::ComposableItem(std::string id, std::string name, std::string description,
                                        std::shared_ptr<SDL_Texture> icon, size_t maxSize) : itemContainer_(
        std::make_unique<ItemContainer>(maxSize)),
    id_(std::move(id)),
    name_(std::move(name)),
    description_(std::move(description)), icon_(std::move(icon)), maxSlotSize_(maxSize) {
}
