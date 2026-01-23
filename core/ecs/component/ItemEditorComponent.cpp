//
// Created by coldmint on 2026/1/23.
//

#include "ItemEditorComponent.h"

void glimmer::ItemEditorComponent::Reserve(const size_t size) {
    slotEntities.reserve(size);
}

void glimmer::ItemEditorComponent::AddSlotEntity(GameEntity::ID id) {
    slotEntities.push_back(id);
}

std::vector<glimmer::GameEntity::ID>& glimmer::ItemEditorComponent::GetSlotEntities() {
    return slotEntities;
}
