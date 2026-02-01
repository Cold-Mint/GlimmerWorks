//
// Created by coldmint on 2026/1/30.
//

#include "ClearCommand.h"
#include "../../world/WorldContext.h"
#include "core/ecs/component/ItemContainerComonent.h"

void glimmer::ClearCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
}

std::string glimmer::ClearCommand::GetName() const {
    return CLEAR_COMMAND_NAME;
}

void glimmer::ClearCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
}

bool glimmer::ClearCommand::Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) {
    if (worldContext_ == nullptr) {
        onMessage(appContext_->GetLangsResources()->worldContextIsNull);
        return false;
    }
    auto playerId = worldContext_->GetPlayerEntity();
    auto *item_container = worldContext_->GetComponent<ItemContainerComponent>(playerId);
    if (item_container == nullptr) {
        onMessage(appContext_->GetLangsResources()->itemContainerIsNull);
        return false;
    }
    item_container->GetItemContainer()->Clear();
    return true;
}

bool glimmer::ClearCommand::RequiresWorldContext() const {
    return true;
}
