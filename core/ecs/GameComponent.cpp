//
// Created by Cold-Mint on 2025/10/26.
//

#include "GameComponent.h"

bool glimmer::GameComponent::isSerializable() {
    return false;
}

std::string glimmer::GameComponent::serialize() {
    return "";
}

void glimmer::GameComponent::deserialize(std::string &data) {
}
