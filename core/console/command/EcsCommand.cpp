//
// Created by coldmint on 2026/1/15.
//

#include "EcsCommand.h"

#include "fmt/base.h"
#include "fmt/compile.h"
#include "../../scene/AppContext.h"
#include "../../scene/WorldScene.h"

std::string glimmer::EcsCommand::EntityToString(const GameEntity::ID gameEntityId) const {
    const std::vector<GameComponent *> components = worldContext_->GetAllComponents(gameEntityId);
    std::string data;
    for (auto &component: components) {
        std::string name = std::to_string(component->GetId());
        switch (component->GetId()) {
            case COMPONENT_ID_AUTO_PICK:
                name = "AutoPick";
                break;
            case COMPONENT_ID_CAMERA:
                name = "Camera";
                break;
            case COMPONENT_ID_DEBUG_DRAW:
                name = "DebugDraw";
                break;
            case COMPONENT_ID_DIGGING:
                name = "digging";
                break;
            case COMPONENT_ID_DROPPED_ITEM:
                name = "droppedItem";
                break;
            case COMPONENT_ID_HOTBAR:
                name = "hotbar";
                break;
            case COMPONENT_ID_ITEM_CONTAINER:
                name = "itemContainer";
                break;
            case COMPONENT_ID_ITEM_SLOT:
                name = "itemSlot";
                break;
            case COMPONENT_ID_MAGNET:
                name = "magnet";
                break;
            case COMPONENT_ID_MAGNETIC:
                name = "magnetic";
                break;
            case COMPONENT_ID_PAUSE:
                name = "pause";
                break;
            case COMPONENT_ID_PLAYER_CONTROL:
                name = "playerControl";
                break;
            case COMPONENT_ID_RIGID_BODY_2D:
                name = "rigidBody2D";
                break;
            case COMPONENT_ID_TILE_LAYER:
                name = "titleLayer";
                break;
            case COMPONENT_ID_TRANSFORM_2D:
                name = "transform2D";
                break;
            case COMPONENT_ID_GUI_TRANSFORM_2D:
                name = "guiTransform2D";
                break;
        }
        data += fmt::format("{}\n", name);
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

bool glimmer::EcsCommand::Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) {
    if (worldContext_ == nullptr) {
        onMessage(appContext_->GetLangsResources()->worldContextIsNull);
        return false;
    }
    int size = commandArgs.GetSize();
    if (commandArgs.GetSize() < 2) {
        onMessage(fmt::format(
            fmt::runtime(appContext_->GetLangsResources()->insufficientParameterLength),
            2, size));
        return false;
    }
    std::string arg = commandArgs.AsString(1);
    if (arg == "showEntityList") {
        for (const std::vector<GameEntity::ID> allGameEntities = worldContext_->GetAllGameEntityId(); const auto &e: allGameEntities) {
            onMessage(EntityToString(e));
        }
        return true;
    }
    if (arg == "displayDetailedInformation") {
        if (commandArgs.GetSize() < 3) {
            onMessage(fmt::format(
                fmt::runtime(appContext_->GetLangsResources()->insufficientParameterLength),
                3, size));
            return false;
        }
        const GameEntity::ID id = commandArgs.AsInt(2);
        if (WorldContext::IsEmptyEntityId(id)) {
            onMessage(appContext_->GetLangsResources()->cantFindObject);
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
