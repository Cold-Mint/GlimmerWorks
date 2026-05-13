//
// Created by Cold-Mint on 2025/11/16.
//

#include "Box2DCommand.h"

#include "../../Constants.h"
#include "../../world/WorldContext.h"
#include "box2d/box2d.h"
#include "fmt/color.h"


void glimmer::Box2DCommand::InitSuggestions(NodeTree<std::string> *suggestionsTree) {
    if (suggestionsTree == nullptr) {
        return;
    }
    suggestionsTree->AddChild("count");
}

glimmer::Box2DCommand::Box2DCommand(AppContext *appContext)
    : Command(appContext) {
}

std::string glimmer::Box2DCommand::GetName() const {
    return BOX2D_COMMAND_NAME;
}

bool glimmer::Box2DCommand::RequiresWorldContext() const {
    return true;
}

void glimmer::Box2DCommand::PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) {
    if (commandArgs == nullptr || strings == nullptr) {
        return;
    }
    strings->emplace_back("count");
}

bool glimmer::Box2DCommand::Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
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
    if (commandArgs->AsString(1) == "count") {
        //Obtain the number of active rigid bodies
        //获取活跃的刚体数量
        const auto bodyCount = b2World_GetAwakeBodyCount(worldContext_->GetWorldId());
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->awakeBodyCount),
            bodyCount));
        return true;
    }
    onMessageRef(langsResources->unknownCommandParameters);
    return false;
}
