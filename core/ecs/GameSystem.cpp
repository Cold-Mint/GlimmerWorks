//
// Created by Cold-Mint on 2025/10/26.
//

#include "GameSystem.h"


bool glimmer::GameSystem::ShouldActivate() {
    if (!worldContext) return false;
    if (!requiredComponents.empty()) {
        for (const auto &type: requiredComponents) {
            if (!worldContext->HasComponentType(type)) {
                //A certain dependent component type in the world does not exist and cannot be activated
                // 世界中某个依赖的组件类型不存在，不能激活
                return false;
            }
        }
    }
    return true; // All components exist and can be activated 所有组件都存在，可以激活
}

void glimmer::GameSystem::OnActivationChanged(const bool activeStatus) {
}

bool glimmer::GameSystem::CheckActivation() {
    const bool shouldActivate = ShouldActivate();
    if (!active && shouldActivate) {
        active = true;
        OnActivationChanged(active);
    } else if (active && shouldActivate) {
        active = false;
        OnActivationChanged(active);
    }
    return active;
}

bool glimmer::GameSystem::IsActive() const {
    return active;
}

bool glimmer::GameSystem::HandleEvent(const SDL_Event &event) {
    return false;
}

void glimmer::GameSystem::Update(float delta) {
}

void glimmer::GameSystem::Render(SDL_Renderer *renderer) {
}
