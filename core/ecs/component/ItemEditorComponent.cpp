//
// Created by Cold-Mint on 2026/1/23.
//

#include "ItemEditorComponent.h"

#include "core/Constants.h"

void glimmer::ItemEditorComponent::Reserve(const size_t size) {
    slotEntities.reserve(size);
}

void glimmer::ItemEditorComponent::AddSlotEntity(GameEntity::ID id) {
    slotEntities.push_back(id);
}

uint32_t glimmer::ItemEditorComponent::GetId() {
    return COMPONENT_ID_ITEM_EDITOR;
}

std::vector<glimmer::GameEntity::ID>& glimmer::ItemEditorComponent::GetSlotEntities() {
    return slotEntities;
}
