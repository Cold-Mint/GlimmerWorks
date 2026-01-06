//
// Created by Cold-Mint on 2026/1/6.
//

#include "SeedCommand.h"
#include "../../world/WorldContext.h"

bool glimmer::SeedCommand::RequiresWorldContext() const {
    return true;
}

bool glimmer::SeedCommand::Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) {
    if (worldContext_ == nullptr) {
        onMessage(appContext_->GetLangsResources()->worldContextIsNull);
        return false;
    }
    auto mapManifest = worldContext_->GetSaves()->ReadMapManifest();
    if (mapManifest.has_value()) {
        onMessage(std::to_string(mapManifest.value().seed()));
        return true;
    }
    onMessage(appContext_->GetLangsResources()->mapManifestIsNull);
    return false;
}

void glimmer::SeedCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
}

std::string glimmer::SeedCommand::GetName() const {
    return SEED_COMMAND_NAME;
}

void glimmer::SeedCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
}
