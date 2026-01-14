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

void glimmer::GameComponent::deserialize(AppContext *appContext, WorldContext *worldContext, std::string &data) {
}

u_int32_t glimmer::GameComponent::GetId() {
    return 0;
}
