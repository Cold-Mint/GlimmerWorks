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

std::string glimmer::TpCommand::GetName() const
{
    return TP_COMMAND_NAME;
}

void glimmer::TpCommand::InitSuggest()
{
}

bool glimmer::TpCommand::Execute(CommandArgs commandArgs, std::function<void(const std::string& text)> onOutput)
{
    if (worldContext_ == nullptr)
    {
        onOutput("WorldContext is nullptr");
        return false;
    }
    auto entities = worldContext_->GetEntitiesWithComponents<
        PlayerControlComponent, RigidBody2DComponent>();
    if (entities.empty())
    {
        onOutput("No entity with RigidBody2DComponent and PlayerControlComponent");
        return false;
    }
    for (auto entity : entities)
    {
        auto rigidBody2DComponent = worldContext_->GetComponent<RigidBody2DComponent>(entity->GetID());
        if (rigidBody2DComponent && rigidBody2DComponent->IsReady())
        {
            b2Vec2 newPos = Box2DUtils::ToMeters({commandArgs.AsFloat(1), commandArgs.AsFloat(2)});
            b2Rot currentRot = b2Body_GetRotation(rigidBody2DComponent->GetBodyId());
            b2Body_SetTransform(rigidBody2DComponent->GetBodyId(), newPos, currentRot);
        }
        onOutput(
            "Teleported entity " + std::to_string(entity->GetID()) + " to x=" + commandArgs.AsString(1) + ",y=" +
            commandArgs.AsString(2));
    }

    return true;
}

bool glimmer::TpCommand::RequiresWorldContext() const
{
    return true;
}
