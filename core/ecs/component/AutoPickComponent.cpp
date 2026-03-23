//
// Created by Cold-Mint on 2025/12/22.
//

#include "AutoPickComponent.h"

#include "core/Constants.h"

uint32_t glimmer::AutoPickComponent::GetId() {
    return COMPONENT_ID_AUTO_PICK;
}

bool glimmer::AutoPickComponent::IsSerializable() {
    return true;
}
