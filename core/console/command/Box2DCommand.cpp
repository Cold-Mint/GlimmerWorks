//
// Created by Cold-Mint on 2025/11/16.
//

#include "Box2DCommand.h"

#include "../../Constants.h"
#include "../../world/WorldContext.h"
#include "box2d/box2d.h"
#include "fmt/color.h"

std::string glimmer::Box2DCommand::GetName() const
{
    return BOX2D_COMMAND_NAME;
}

bool glimmer::Box2DCommand::RequiresWorldContext() const
{
    return true;
}

bool glimmer::Box2DCommand::Execute(CommandArgs commandArgs, std::function<void(const std::string& text)> onOutput)
{
    const auto type = commandArgs.AsString(1);
    if (type == "count")
    {
        //Obtain the number of active rigid bodies
        //获取活跃的刚体数量
        const auto bodyCount = b2World_GetAwakeBodyCount(worldContext_->GetWorldId());
        onOutput(fmt::format(
            fmt::runtime(appContext_->langs->awakeBodyCount),
            bodyCount));
    }
    return true;
}


void glimmer::Box2DCommand::InitSuggest()
{
}
