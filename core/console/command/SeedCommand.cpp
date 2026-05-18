//
// Created by Cold-Mint on 2026/1/6.
//

#include "SeedCommand.h"
#include "core/world/TileInstancePool.h"
#include "core/world/WorldContext.h"

void glimmer::SeedCommand::InitSuggestions(NodeTree<std::string> *suggestionsTree) {

}

glimmer::SeedCommand::SeedCommand(AppContext *ctx)
    : Command(ctx) {
}

bool glimmer::SeedCommand::RequiresWorldContext() const {
    return true;
}

bool glimmer::SeedCommand::Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                                   const std::function<void(const std::string &text)> *onMessage) {
    if (appContext_ == nullptr) {
        return false;
    }
    const std::function<void(const std::string &text)> &onMessageRef = *onMessage;
    if (worldContext_ == nullptr) {
        onMessageRef(appContext_->GetLangsResources()->worldContextIsNull);
        return false;
    }
    onMessageRef(std::to_string(worldContext_->GetWorldSeed()));
    return true;
}

std::string glimmer::SeedCommand::GetName() const {
    return SEED_COMMAND_NAME;
}

void glimmer::SeedCommand::PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) {

}
