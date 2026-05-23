//
// Created by coldmint on 2026/5/23.
//

#include "BlueprintComponent.h"

void glimmer::BlueprintComponent::SetCanPlace(bool canPlace) {
    canPlace_ = canPlace;
}

bool glimmer::BlueprintComponent::CanPlace() const {
    return canPlace_;
}

GameComponentTypeMessage glimmer::BlueprintComponent::GetComponentType() {
    return COMPONENT_BLUEPRINT;
}
