//
// Created by Cold-Mint on 2026/5/23.
//

#include "BlueprintComponent.h"

void glimmer::BlueprintComponent::SetCanPlace(bool canPlace) {
    canPlace_ = canPlace;
}

bool glimmer::BlueprintComponent::CanPlace() const {
    return canPlace_;
}

void glimmer::BlueprintComponent::SetTopLeftVector(const TileVector2D &topLeftVector) {
    topLeftVector_ = topLeftVector;
}

TileVector2D glimmer::BlueprintComponent::GetTopLeftVector() const {
    return topLeftVector_;
}

GameComponentTypeMessage glimmer::BlueprintComponent::GetComponentType() {
    return COMPONENT_BLUEPRINT;
}
