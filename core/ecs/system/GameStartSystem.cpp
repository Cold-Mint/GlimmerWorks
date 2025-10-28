//
// Created by Cold-Mint on 2025/10/28.
//

#include "GameStartSystem.h"

#include "../component/WorldPositionComponent.h"


void glimmer::GameStartSystem::Update(float delta) {
    LogCat::d("Game Start System init");
    auto e = worldContext->CreateEntity();
    worldContext->AddComponent<WorldPositionComponent>(e);
    // worldContext->RemoveComponent<WorldPositionComponent>(e->GetID());
    worldContext->RemoveEntity(e->GetID());
}

std::string glimmer::GameStartSystem::GetName() {
    return "glimmer.GameStartSystem";
}

bool glimmer::GameStartSystem::ShouldActivate() {
    if (shouldStart) {
        shouldStart = false;
        return true;
    }
    return false;
}
