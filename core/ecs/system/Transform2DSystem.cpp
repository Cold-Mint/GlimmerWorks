//
// Created by Cold-Mint on 2025/10/28.
//

#include "Transform2DSystem.h"
#include "../component/Transform2DComponent.h"

glimmer::Transform2DSystem::Transform2DSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent<Transform2DComponent>();
}

std::string glimmer::Transform2DSystem::GetName() {
    return "glimmer.Transform2DSystem";
}
