//
// Created by Cold-Mint on 2025/11/16.
//

#include "TpCommand.h"

#include "../../Constants.h"
#include "../../ecs/component/RigidBody2DComponent.h"
#include "../../utils/Box2DUtils.h"
#include "../../world/WorldContext.h"
#include "box2d/box2d.h"
#include "fmt/color.h"

void glimmer::TpCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
    suggestionsTree.AddChild(X_DYNAMIC_SUGGESTIONS_NAME)->AddChild(Y_DYNAMIC_SUGGESTIONS_NAME);
}

glimmer::TpCommand::TpCommand(AppContext *appContext) : Command(appContext) {
}

std::string glimmer::TpCommand::GetName() const {
    return TP_COMMAND_NAME;
}

bool glimmer::TpCommand::Execute(const CommandSender *commandSender, CommandArgs commandArgs,
                                 std::function<void(const std::string &text)> onMessage) {
    if (appContext_ == nullptr) {
        return false;
    }
    const LangsResources *langsResources = appContext_->GetLangsResources();
    if (langsResources == nullptr) {
        return false;
    }
    if (worldContext_ == nullptr) {
        onMessage(langsResources->worldContextIsNull);
        return false;
    }

    const size_t size = commandArgs.GetSize();
    if (size < 2) {
        onMessage(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
        return false;
    }
    auto playerEntity = worldContext_->GetPlayerEntity();
    if (WorldContext::IsEmptyEntityId(playerEntity)) {
        onMessage(langsResources->cantFindObject);
        return false;
    }
    const WorldVector2D commandSenderPosition = commandSender->GetPosition();
    auto rigidBody2DComponent = worldContext_->GetComponent<RigidBody2DComponent>(playerEntity);
    if (rigidBody2DComponent && rigidBody2DComponent->IsReady()) {
        b2Vec2 newPos = Box2DUtils::ToMeters({
            commandArgs.AsCoordinate(1, commandSenderPosition.x),
            commandArgs.AsCoordinate(2, commandSenderPosition.y)
        });
        b2Rot currentRot = b2Body_GetRotation(rigidBody2DComponent->GetBodyId());
        b2Body_SetTransform(rigidBody2DComponent->GetBodyId(), newPos, currentRot);
    }
    onMessage(fmt::format(
        fmt::runtime(langsResources->teleportEntity),
        playerEntity, commandArgs.AsString(1), commandArgs.AsString(2)));

    return true;
}

void glimmer::TpCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
    strings.emplace_back("[x:int]");
    strings.emplace_back("[y:int]");
}

bool glimmer::TpCommand::RequiresWorldContext() const {
    return true;
}
