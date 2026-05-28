//
// Created by Cold-Mint on 2026/3/28.
//

#include "FlyCommand.h"

#include "box2d/box2d.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/ecs/component/RigidBody2DComponent.h"
#include "core/utils/Box2DUtils.h"
#include "core/world/WorldContext.h"
#include "fmt/xchar.h"

void glimmer::FlyCommand::InitSuggestions(NodeTree<std::string> *suggestionsTree) {
    if (suggestionsTree == nullptr) {
        return;
    }
    suggestionsTree->AddChild(BOOL_TOGGLE_DYNAMIC_SUGGESTIONS_NAME);
}

glimmer::FlyCommand::FlyCommand(AppContext *appContext) : Command(appContext) {
}

std::string glimmer::FlyCommand::GetName() const {
    return FLY_COMMAND_NAME;
}

bool glimmer::FlyCommand::RequiresWorldContext() const {
    return true;
}

void glimmer::FlyCommand::PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) {
    if (strings == nullptr) {
        return;
    }
    strings->emplace_back("[enable:bool]");
}

bool glimmer::FlyCommand::Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                                  const std::function<void(const std::string &text)> *onMessage) {
    if (appContext_ == nullptr || commandArgs == nullptr || onMessage == nullptr) {
        return false;
    }
    const std::function<void(const std::string &text)> &onMessageRef = *onMessage;
    const LangsResources *langsResources = appContext_->GetLangsResources();
    if (langsResources == nullptr) {
        return false;
    }
    if (worldContext_ == nullptr) {
        onMessageRef(langsResources->worldContextIsNull);
        return false;
    }
    int size = commandArgs->GetSize();
    if (size < 2) {
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
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
    BoolOrToggle boolOrToggle = commandArgs->AsBoolOrToggle(1);
    bool newValue = boolOrToggle == TRUE;
    if (boolOrToggle == TOGGLE) {
        newValue = !playerComponent->isFlying;
    }
    if (newValue) {
        auto rigidBody2DComponent = worldContext_->GetComponent<RigidBody2DComponent>(playerEntity);
        if (rigidBody2DComponent != nullptr) {
            rigidBody2DComponent->Disable();
        }
        onMessageRef(langsResources->flyEnable);
    } else {
        auto transform2DComponent = worldContext_->GetComponent<Transform2DComponent>(playerEntity);
        if (transform2DComponent != nullptr) {
            auto rigidBody2DComponent = worldContext_->GetComponent<RigidBody2DComponent>(playerEntity);
            if (rigidBody2DComponent && rigidBody2DComponent->IsReady()) {
                rigidBody2DComponent->Enable();
                b2Vec2 newPos = Box2DUtils::ToMeters(transform2DComponent->GetPosition());
                b2BodyId bodyId = rigidBody2DComponent->GetBodyId();
                b2Rot currentRot = b2Body_GetRotation(bodyId);
                b2Body_SetTransform(bodyId, newPos, currentRot);
            }
        }
        onMessageRef(langsResources->flyDisable);
    }
    playerComponent->isFlying = newValue;
    return true;
}
