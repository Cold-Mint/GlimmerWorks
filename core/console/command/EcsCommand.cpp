//
// Created by Cold-Mint on 2026/1/15.
//

#include "EcsCommand.h"

#include "fmt/base.h"
#include "fmt/compile.h"
#include "../../scene/AppContext.h"
#include "../../scene/WorldScene.h"

glimmer::EcsCommand::EcsCommand(AppContext *appContext) : Command(appContext) {
}

std::string glimmer::EcsCommand::EntityToString(const GameEntity::ID gameEntityId) const {
    const std::vector<GameComponent *> components = worldContext_->GetAllComponents(gameEntityId);
    std::string data;
    for (auto &component: components) {
        data += fmt::format("componentId = {} name = {}\n", component->GetId(), typeid(component).name());
    }
    return fmt::format("id={} isPersistable={} \ncomponents={}\n", gameEntityId,
                       worldContext_->IsPersistable(gameEntityId),
                       data);
}

void glimmer::EcsCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
    suggestionsTree.AddChild("showEntityList");
    suggestionsTree.AddChild("displayDetailedInformation");
    suggestionsTree.AddChild("activeSystems");
}

std::string glimmer::EcsCommand::GetName() const {
    return ECS_COMMAND_NAME;
}

void glimmer::EcsCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
    strings.emplace_back("[type:string]");
    if (commandArgs.GetSize() > 1 && commandArgs.AsString(1) == "displayDetailedInformation") {
        strings.emplace_back("[id:uint]");
    }
}

bool glimmer::EcsCommand::Execute(const CommandArgs commandArgs, const std::function<void(const std::string &text)> onMessage) {
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
    int size = commandArgs.GetSize();
    if (commandArgs.GetSize() < 2) {
        onMessage(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
        return false;
    }
    std::string arg = commandArgs.AsString(1);
    if (arg == "showEntityList") {
        for (const std::vector<GameEntity::ID> allGameEntities = worldContext_->GetAllGameEntityId(); const auto &e:
             allGameEntities) {
            onMessage(EntityToString(e));
        }
        return true;
    }
    if (arg == "displayDetailedInformation") {
        if (commandArgs.GetSize() < 3) {
            onMessage(fmt::format(
                fmt::runtime(langsResources->insufficientParameterLength),
                3, size));
            return false;
        }
        const GameEntity::ID id = commandArgs.AsInt(2);
        if (WorldContext::IsEmptyEntityId(id)) {
            onMessage(langsResources->cantFindObject);
            return false;
        }
        onMessage(EntityToString(id));
        return true;
    }
    if (arg == "activeSystems") {
        std::vector<GameSystem *> allGameSystems = worldContext_->GetAllActiveSystem();
        for (auto &s: allGameSystems) {
            onMessage(fmt::format("{}\n", s->GetName()));
        }
        return true;
    }
    return false;
}

bool glimmer::EcsCommand::RequiresWorldContext() const {
    return true;
}
