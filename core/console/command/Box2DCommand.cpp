//
// Created by Cold-Mint on 2025/11/16.
//

#include "Box2DCommand.h"

#include "../../Constants.h"
#include "../../world/WorldContext.h"
#include "box2d/box2d.h"
#include "fmt/color.h"

void glimmer::Box2DCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
    suggestionsTree.AddChild("count");
}

std::string glimmer::Box2DCommand::GetName() const {
    return BOX2D_COMMAND_NAME;
}

bool glimmer::Box2DCommand::RequiresWorldContext() const {
    return true;
}

bool glimmer::Box2DCommand::Execute(const CommandArgs commandArgs,
                                    const std::function<void(const std::string &text)> onMessage) {
    if (worldContext_ == nullptr) {
        onMessage(appContext_->GetLangsResources()->worldContextIsNull);
        return false;
    }
    if (commandArgs.AsString(1) == "count") {
        //Obtain the number of active rigid bodies
        //获取活跃的刚体数量
        const auto bodyCount = b2World_GetAwakeBodyCount(worldContext_->GetWorldId());
        onMessage(fmt::format(
            fmt::runtime(appContext_->GetLangsResources()->awakeBodyCount),
            bodyCount));
        return true;
    }
    onMessage(appContext_->GetLangsResources()->unknownCommandParameters);
    return false;
}

void glimmer::Box2DCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
    strings.emplace_back("count");
}
