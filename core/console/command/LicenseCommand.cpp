//
// Created by Cold-Mint on 2025/12/9.
//

#include "LicenseCommand.h"

#include "../../Constants.h"

bool glimmer::LicenseCommand::Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) {
    auto text = virtualFileSystem_->ReadFile("LICENSE");
    if (text.has_value()) {
        onMessage(text.value());
    } else {
        onMessage(
            "Failed to load the local LICENSE file.\nThis file should be included with every distribution of this program.\nIf it is missing, please check your installation or view the license online at:\nhttps://github.com/Cold-Mint/GlimmerWorks/blob/master/LICENSE");
    }
    return true;
}

void glimmer::LicenseCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
}

void glimmer::LicenseCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
}

std::string glimmer::LicenseCommand::GetName() const {
    return LICENSE_COMMAND_NAME;
}
