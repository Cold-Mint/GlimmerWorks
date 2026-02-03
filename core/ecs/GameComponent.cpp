//
// Created by Cold-Mint on 2025/10/26.
//

#include "GameComponent.h"

bool glimmer::GameComponent::IsSerializable() {
    return false;
}

std::string glimmer::GameComponent::Serialize() {
    return "";
}

void glimmer::GameComponent::Deserialize(WorldContext *worldContext, const std::string &data) {
}

u_int32_t glimmer::GameComponent::GetId() {
    return 0;
}
