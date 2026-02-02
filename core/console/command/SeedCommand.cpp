//
// Created by Cold-Mint on 2026/1/6.
//

#include "SeedCommand.h"
#include "../../world/WorldContext.h"

glimmer::SeedCommand::SeedCommand(AppContext *ctx)
    : Command(ctx) {
}

bool glimmer::SeedCommand::RequiresWorldContext() const {
    return true;
}

bool glimmer::SeedCommand::Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) {
    if (worldContext_ == nullptr) {
        onMessage(appContext_->GetLangsResources()->worldContextIsNull);
        return false;
    }
    onMessage(std::to_string(worldContext_->GetSeed()));
    return true;
}

void glimmer::SeedCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
}

std::string glimmer::SeedCommand::GetName() const {
    return SEED_COMMAND_NAME;
}

void glimmer::SeedCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
}
