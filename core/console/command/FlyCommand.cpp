//
// Created by coldmint on 2026/3/28.
//

#include "FlyCommand.h"

#include "core/ecs/component/PlayerComponent.h"
#include "core/ecs/component/RigidBody2DComponent.h"
#include "core/world/WorldContext.h"

glimmer::FlyCommand::FlyCommand(AppContext *appContext) : Command(appContext) {
}

void glimmer::FlyCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
}

std::string glimmer::FlyCommand::GetName() const {
    return FLY_COMMAND_NAME;
}

bool glimmer::FlyCommand::RequiresWorldContext() const {
    return true;
}

void glimmer::FlyCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
}

bool glimmer::FlyCommand::Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) {
    if (worldContext_ == nullptr) {
        return false;
    }
    const GameEntity::ID playerEntity = worldContext_->GetPlayerEntity();
    if (WorldContext::IsEmptyEntityId(playerEntity)) {
        return false;
    }
    auto playerComponent = worldContext_->GetComponent<PlayerComponent>(playerEntity);
    if (playerComponent == nullptr) {
        return false;
    }
    if (playerComponent->isFlying) {
        auto rigidBody2DComponent = worldContext_->GetComponent<RigidBody2DComponent>(playerEntity);
        if (rigidBody2DComponent != nullptr) {
            rigidBody2DComponent->Enable();
        }
    } else {
        auto rigidBody2DComponent = worldContext_->GetComponent<RigidBody2DComponent>(playerEntity);
        if (rigidBody2DComponent != nullptr) {
            rigidBody2DComponent->Disable();
        }
    }
    playerComponent->isFlying = !playerComponent->isFlying;
    return true;
}
