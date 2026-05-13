//
// Created by Cold-Mint on 2025/12/9.
//

#include "LicenseCommand.h"

#include "../../Constants.h"
#include "../../scene/AppContext.h"

void glimmer::LicenseCommand::InitSuggestions(NodeTree<std::string> *suggestionsTree) {
}

glimmer::LicenseCommand::LicenseCommand(AppContext *appContext) : Command(appContext) {
}

bool glimmer::LicenseCommand::Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                                      const std::function<void(const std::string &text)> *onMessage) {
    if (appContext_ == nullptr) {
        return false;
    }
    const std::function<void(const std::string &text)> &onMessageRef = *onMessage;
    if (const auto text = appContext_->GetVirtualFileSystem()->ReadFile("LICENSE"); text.has_value()) {
        onMessageRef(text.value());
        return true;
    }
    onMessageRef(
        appContext_->GetLangsResources()->failedToLoadLicense);
    return false;
}

void glimmer::LicenseCommand::PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) {
}

std::string glimmer::LicenseCommand::GetName() const {
    return LICENSE_COMMAND_NAME;
}
