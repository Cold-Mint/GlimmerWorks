//
// Created by Cold-Mint on 2025/12/18.
//

#include "HotBarComonent.h"

#include "core/Constants.h"


glimmer::HotBarComponent::HotBarComponent(const int maxSlot) : maxSlot_(maxSlot) {
}

int glimmer::HotBarComponent::GetMaxSlot() const {
    return maxSlot_;
}

void glimmer::HotBarComponent::AddSlotEntity(const GameEntity::ID entity) {
    slotEntities_.push_back(entity);
}

const std::vector<glimmer::GameEntity::ID> &glimmer::HotBarComponent::GetSlotEntities() const {
    return slotEntities_;
}

uint32_t glimmer::HotBarComponent::GetId() {
    return COMPONENT_ID_HOTBAR;
}
