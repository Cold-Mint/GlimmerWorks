//
// Created by Cold-Mint on 2025/10/26.
//

#include "GameSystem.h"
#include "../world/WorldContext.h"

bool glimmer::GameSystem::ShouldActivate() {
    if (worldContext_ == nullptr) {
        return false;
    }
    if (requiredComponents.empty()) {
        return false;
    }
    for (const auto &type: requiredComponents) {
        if (!worldContext_->HasComponentType(type)) {
            //A certain dependent component type in the world does not exist and cannot be activated
            // 世界中某个依赖的组件类型不存在，不能激活
            return false;
        }
    }
    return true; // All components exist and can be activated 所有组件都存在，可以激活
}

void glimmer::GameSystem::OnActivationChanged(const bool activeStatus) {
}


glimmer::GameSystem::GameSystem(AppContext *appContext, WorldContext *worldContext) : worldContext_(worldContext),
    appContext_(appContext) {
}

bool glimmer::GameSystem::CheckActivation() {
    const bool shouldActivate = ShouldActivate();
    if (!active && shouldActivate) {
        active = true;
        LogCat::w("System Activated: ", GetName());
        OnActivationChanged(active);
    } else if (active && !shouldActivate) {
        active = false;
        LogCat::w("System Deactivated: ", GetName());
        OnActivationChanged(active);
    }
    return active;
}

bool glimmer::GameSystem::IsActive() const {
    return active;
}

bool glimmer::GameSystem::SupportsComponentType(const std::type_index &type) const {
    if (requiredComponents.empty()) {
        return false;
    }
    return requiredComponents.contains(type);
}

bool glimmer::GameSystem::CanRunWhilePaused() const {
    return false;
}

bool glimmer::GameSystem::HandleEvent(const SDL_Event &event) {
    return false;
}

bool glimmer::GameSystem::OnBackPressed() {
    return false;
}

void glimmer::GameSystem::Update(float delta) {
}

uint8_t glimmer::GameSystem::GetRenderOrder() {
    return 0;
}

void glimmer::GameSystem::Render(SDL_Renderer *renderer) {
}
