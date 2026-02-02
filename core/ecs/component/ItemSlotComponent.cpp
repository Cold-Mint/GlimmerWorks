//
// Created by Cold-Mint on 2025/12/25.
//

#include "ItemSlotComponent.h"

#include "core/Constants.h"


glimmer::ItemSlotComponent::ItemSlotComponent(ItemContainer *itemContainer, const int slotIndex)
    : itemContainer_(itemContainer), slotIndex_(slotIndex) {
}

glimmer::ItemContainer *glimmer::ItemSlotComponent::GetItemContainer() const {
    return itemContainer_;
}

int glimmer::ItemSlotComponent::GetSlotIndex() const {
    return slotIndex_;
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

u_int32_t glimmer::ItemSlotComponent::GetId() {
    return COMPONENT_ID_ITEM_SLOT;
}
