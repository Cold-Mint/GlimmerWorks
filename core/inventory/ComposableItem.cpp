//
// Created by Cold-Mint on 2025/12/23.
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
#include "core/ecs/DroppedItemCreator.h"

void glimmer::ComposableItem::SwapItem(size_t index, ItemContainer *otherContainer, size_t otherIndex) const {
    itemContainer_->SwapItem(index, otherContainer, otherIndex);
}

void glimmer::ComposableItem::RefreshAttributes() {
    LogCat::d("ComposableItem RefreshAttributes");
    const size_t max = itemContainer_->GetCapacity();
    totalAbilityConfig_.Reset();
    for (size_t index = 0; index < max; index++) {
        Item *item = itemContainer_->GetItem(index);
        if (item == nullptr) {
            continue;
        }
        auto *abilityItem = dynamic_cast<AbilityItem *>(item);
        if (abilityItem == nullptr) {
            continue;
        }
        totalAbilityConfig_ += item->GetAbilityConfig();
    }
}

std::unique_ptr<glimmer::Item> glimmer::ComposableItem::ReplaceItem(const size_t index,
                                                                    std::unique_ptr<Item> item) const {
    return itemContainer_->ReplaceItem(index, std::move(item));
}

size_t glimmer::ComposableItem::RemoveItemAbility(const std::string &id, const size_t amount) const {
    return itemContainer_->RemoveItem(id, amount);
}


std::unique_ptr<glimmer::ComposableItem> glimmer::ComposableItem::FromItemResource(const AppContext *appContext,
    const ComposableItemResource *itemResource, const ResourceRef &resourceRef) {
    std::string name = Resource::GenerateId(itemResource->packId, itemResource->resourceId);
    const auto nameRes = appContext->GetResourceLocator()->FindString(itemResource->name);
    if (nameRes != nullptr) {
        name = nameRes->value;
    }
    std::optional<std::string> description;
    auto descriptionRes = appContext->GetResourceLocator()->FindString(itemResource->description);
    if (descriptionRes != nullptr) {
        description = descriptionRes->value;
    }
    std::unique_ptr<ComposableItem> result = std::make_unique<ComposableItem>(
        Resource::GenerateId(*itemResource), name,
        description,
        appContext->GetResourceLocator()->FindTexture(itemResource->texture),
        itemResource->slotSize, resourceRef);
    //If the capability is not specified within the resource reference, then the default capability will be loaded.
    //如果没有在资源引用内指定能力，那么加载默认能力。
    size_t defaultAbilitySize = itemResource->defaultAbilityList.size();
    if (defaultAbilitySize > 0) {
        for (int i = 0; i < defaultAbilitySize; i++) {
            auto itemObj = appContext->GetResourceLocator()->FindItem(
                itemResource->defaultAbilityList[i]);
            if (itemObj != nullptr) {
                (void) result->ReplaceItem(static_cast<size_t>(i), std::move(itemObj));
            }
        }
    }
    return result;
}


const glimmer::AbilityConfig &glimmer::ComposableItem::GetAbilityConfig() const {
    return totalAbilityConfig_;
}

void glimmer::ComposableItem::OnUse(WorldContext *worldContext, GameEntity::ID user, const AbilityConfig &configMessage,
                                    std::unordered_set<std::string> &popupAbility) {
    const size_t max = itemContainer_->GetCapacity();
    //The ability to pop up
    //需要弹出的能力
    for (size_t index = 0; index < max; index++) {
        Item *item = itemContainer_->GetItem(index);
        if (item == nullptr) {
            continue;
        }
        auto *abilityItem = dynamic_cast<AbilityItem *>(item);
        if (abilityItem == nullptr) {
            continue;
        }
        ItemAbility *itemAbility = abilityItem->GetItemAbility();
        if (itemAbility == nullptr) {
            continue;
        }

        if (popupAbility.contains(itemAbility->GetId())) {
            //Mutual exclusivity
            //互斥
            const GameEntity::ID droppedEntity = worldContext->CreateEntity();
            DroppedItemCreator droppedItemCreator{worldContext};
            droppedItemCreator.LoadTemplateComponents(droppedEntity, DroppedItemCreator::GetResourceRef());
            droppedItemCreator.MergeEntityItemMessage(droppedEntity, DroppedItemCreator::GetEntityItemMessage(
                                                          worldContext->GetCameraTransform2D()->
                                                          GetPosition(), itemContainer_->TakeAllItem(index), 2));
            continue;
        }
        itemAbility->OnUse(worldContext, user, configMessage, popupAbility);
    }
}

glimmer::ItemContainer *glimmer::ComposableItem::GetItemContainer() const {
    return itemContainer_.get();
}

std::unique_ptr<glimmer::Item> glimmer::ComposableItem::Clone() const {
    LogCat::d("ComposableItem Clone");
    auto composableItem = std::make_unique<ComposableItem>(*this);
    composableItem->AddCallback();
    return composableItem;
}

void glimmer::ComposableItem::AddCallback() {
    LogCat::d("ComposableItem AddCallback");
    callback_ = itemContainer_->AddOnContentChanged([this](ContainerChangeType changeType) {
        switch (changeType) {
            case ContainerChangeType::ADD:
                LogCat::d("ComposableItem changeType add");
                RefreshAttributes();
                break;
            case ContainerChangeType::REMOVE:
                LogCat::d("ComposableItem changeType remove");
                RefreshAttributes();
                break;
            default:
                LogCat::d("ComposableItem changeType default");
        }
    });
}

glimmer::ComposableItem::ComposableItem(std::string id, std::string name, std::optional<std::string> description,
                                        std::shared_ptr<SDL_Texture> icon, size_t maxSize,
                                        const ResourceRef &resourceRef) : itemContainer_(
                                                                              std::make_shared<ItemContainer>(maxSize)),
                                                                          id_(std::move(id)),
                                                                          name_(std::move(name)),
                                                                          description_(std::move(description)),
                                                                          icon_(std::move(icon)),
                                                                          maxSlotSize_(maxSize) {
    resourceRef_ = resourceRef;
    AddCallback();
    LogCat::d("ComposableItem Constructor");
}

void glimmer::ComposableItem::ReadItemMessage(const AppContext *context, const ItemMessage &itemMessage) {
    Item::ReadItemMessage(context, itemMessage);
    //Filling ability.
    //填充能力。
    const ResourceLocator *resourceLocator = context->GetResourceLocator();
    auto abilityItemRefSize = itemMessage.abilityitemref_size();
    for (int i = 0; i < maxSlotSize_; i++) {
        if (i >= abilityItemRefSize) {
            break;
        }
        const ItemMessage &abilityItemMessage = itemMessage.abilityitemref(i);
        std::unique_ptr<Item> item = resourceLocator->FindItem(abilityItemMessage);
        if (item != nullptr) {
            std::unique_ptr<Item> result = ReplaceItem(static_cast<size_t>(i), std::move(item));
        }
    }
}

void glimmer::ComposableItem::WriteItemMessage(ItemMessage &itemMessage) const {
    Item::WriteItemMessage(itemMessage);
    itemMessage.clear_abilityitemref();
    for (int i = 0; i < maxSlotSize_; i++) {
        ItemMessage *abilityItemMessage = itemMessage.add_abilityitemref();
        const Item *item = itemContainer_->GetItem(i);
        if (item == nullptr) {
            continue;
        }
        item->WriteItemMessage(*abilityItemMessage);
    }
}

glimmer::ComposableItem::~ComposableItem() {
    itemContainer_->RemoveOnContentChanged(callback_);
    LogCat::d("ComposableItem Destroy");
}

const std::string &glimmer::ComposableItem::GetId() const {
    return id_;
}

const std::string &glimmer::ComposableItem::GetName() const {
    return name_;
}

const std::optional<std::string> &glimmer::ComposableItem::GetDescription() const {
    return description_;
}

SDL_Texture *glimmer::ComposableItem::GetIcon() const {
    return icon_.get();
}
