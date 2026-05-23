//
// Created by Cold-Mint on 2025/12/22.
//

#include "MagneticComponent.h"

#include "core/Constants.h"

void glimmer::MagneticComponent::SetType(uint16_t type) {
    type_ = type;
}


const WorldVector2D &glimmer::MagneticComponent::GetStartPos() const {
    return startPos_;
}

void glimmer::MagneticComponent::SetStartPos(const WorldVector2D &startPos) {
    startPos_ = startPos;
}

uint16_t glimmer::MagneticComponent::GetType() const {
    return type_;
}

GameComponentTypeMessage glimmer::MagneticComponent::GetComponentType() {
    return COMPONENT_MAGNET;
}


bool glimmer::MagneticComponent::IsSerializable() {
    return false;
}
