//
// Created by Cold-Mint on 2025/12/9.
//

#include "LicenseCommand.h"

#include "../../Constants.h"
#include "../../scene/AppContext.h"

glimmer::LicenseCommand::LicenseCommand(AppContext *ctx, VirtualFileSystem *virtualFileSystem)
    : Command(ctx), virtualFileSystem_(virtualFileSystem) {
}

bool glimmer::LicenseCommand::Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) {
    if (const auto text = virtualFileSystem_->ReadFile("LICENSE"); text.has_value()) {
        onMessage(text.value());
        return true;
    }
    onMessage(
        appContext_->GetLangsResources()->failedToLoadLicense);
    return false;
}

void glimmer::LicenseCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
}

void glimmer::LicenseCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
}

std::string glimmer::LicenseCommand::GetName() const {
    return LICENSE_COMMAND_NAME;
}
