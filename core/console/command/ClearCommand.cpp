//
// Created by Cold-Mint on 2026/1/30.
//

#include "ClearCommand.h"
#include "../../world/WorldContext.h"
#include "core/ecs/component/ItemContainerComonent.h"

void glimmer::ClearCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
}

glimmer::ClearCommand::ClearCommand(AppContext *appContext) : Command(appContext) {
}

std::string glimmer::ClearCommand::GetName() const {
    return CLEAR_COMMAND_NAME;
}

void glimmer::ClearCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
}

bool glimmer::ClearCommand::Execute(CommandArgs commandArgs,
                                    const std::function<void(const std::string &text)> onMessage) {
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
    const auto playerEntity = worldContext_->GetPlayerEntity();
    const auto *itemContainerComponent = worldContext_->GetComponent<ItemContainerComponent>(playerEntity);
    if (itemContainerComponent == nullptr) {
        onMessage(langsResources->itemContainerIsNull);
        return false;
    }
    ItemContainer *itemContainer = itemContainerComponent->GetItemContainer();
    if (itemContainer == nullptr) {
        return false;
    }
    itemContainer->ResetItems();
    return true;
}

bool glimmer::ClearCommand::RequiresWorldContext() const {
    return true;
}
