//
// Created by Cold-Mint on 2025/12/22.
//

#include "AutoPickComponent.h"

GameComponentTypeMessage glimmer::AutoPickComponent::GetComponentType() {
    return COMPONENT_AUTO_PICK;
}

bool glimmer::AutoPickComponent::IsSerializable() {
    return true;
}
