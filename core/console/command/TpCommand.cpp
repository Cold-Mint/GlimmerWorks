//
// Created by Cold-Mint on 2025/11/16.
//

#include "TpCommand.h"

#include "../../Constants.h"
#include "../../ecs/component/PlayerControlComponent.h"
#include "../../ecs/component/RigidBody2DComponent.h"
#include "../../log/LogCat.h"
#include "../../utils/Box2DUtils.h"
#include "../../world/WorldContext.h"
#include "fmt/color.h"

void glimmer::TpCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
}

std::string glimmer::TpCommand::GetName() const {
    return TP_COMMAND_NAME;
}

bool glimmer::TpCommand::Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) {
    if (worldContext_ == nullptr) {
        return false;
    }
    auto entities = worldContext_->GetEntitiesWithComponents<
        PlayerControlComponent, RigidBody2DComponent>();
    if (entities.empty()) {
        onMessage(appContext_->GetLangsResources()->cantFindObject);
        return false;
    }
    for (auto entity: entities) {
        auto rigidBody2DComponent = worldContext_->GetComponent<RigidBody2DComponent>(entity->GetID());
        if (rigidBody2DComponent && rigidBody2DComponent->IsReady()) {
            b2Vec2 newPos = Box2DUtils::ToMeters({
                commandArgs.AsFloat(1) * TILE_SIZE, commandArgs.AsFloat(2) * TILE_SIZE
            });
            b2Rot currentRot = b2Body_GetRotation(rigidBody2DComponent->GetBodyId());
            b2Body_SetTransform(rigidBody2DComponent->GetBodyId(), newPos, currentRot);
        }
        onMessage(fmt::format(
            fmt::runtime(appContext_->GetLangsResources()->teleportEntity),
            entity->GetID(), commandArgs.AsString(1), commandArgs.AsString(2)));
    }

    return true;
}

void glimmer::TpCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
    strings.emplace_back("[x:int]");
    strings.emplace_back("[y:int]");
}

bool glimmer::TpCommand::RequiresWorldContext() const {
    return true;
}
