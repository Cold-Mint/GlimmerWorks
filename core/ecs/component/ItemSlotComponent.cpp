//
// Created by Cold-Mint on 2025/12/25.
//

#include "ItemSlotComponent.h"

#include "core/Constants.h"


glimmer::ItemSlotComponent::ItemSlotComponent(ItemContainer *itemContainer, int slotIndex,
                                              bool allowSelected) : itemContainer_(itemContainer),
                                                                    slotIndex_(slotIndex),
                                                                    allowSelected_(allowSelected) {
}

bool glimmer::ItemSlotComponent::AllowSelected() const {
    return allowSelected_;
}

glimmer::Item *glimmer::ItemSlotComponent::GetItem() const {
    return itemContainer_->GetItem(slotIndex_);
}

std::unique_ptr<glimmer::Item> glimmer::ItemSlotComponent::TakeAllItem() const {
    return itemContainer_->TakeAllItem(slotIndex_);
}

std::unique_ptr<glimmer::Item> glimmer::ItemSlotComponent::ReplaceItem(std::unique_ptr<Item> item) const {
    return itemContainer_->ReplaceItem(slotIndex_, std::move(item));
}


bool glimmer::ItemSlotComponent::IsHovered() const {
    return isHovered_;
}

void glimmer::ItemSlotComponent::SetHovered(const bool hovered) {
    isHovered_ = hovered;
}

bool glimmer::ItemSlotComponent::IsSelected() const {
    return isSelected_;
}

void glimmer::ItemSlotComponent::SetSelected(const bool selected) {
    isSelected_ = selected;
}

uint32_t glimmer::ItemSlotComponent::GetId() {
    return COMPONENT_ID_ITEM_SLOT;
}
