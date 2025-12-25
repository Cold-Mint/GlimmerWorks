//
// Created by Cold-Mint on 2025/12/25.
//

#include "ItemSlotComponent.h"

glimmer::GameEntity *glimmer::ItemSlotComponent::GetContainerEntity() const {
    return containerEntity_;
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
