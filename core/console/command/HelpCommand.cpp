//
// Created by Cold-Mint on 2025/10/15.
//

#include "HelpCommand.h"

#include "../../Constants.h"
#include "../../scene/AppContext.h"


void glimmer::HelpCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
}

glimmer::HelpCommand::HelpCommand(AppContext *appContext) : Command(appContext) {
}

std::string glimmer::HelpCommand::GetName() const {
    return HELP_COMMAND_NAME;
}

void glimmer::HelpCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
}

bool glimmer::HelpCommand::Execute(const CommandSender *commandSender, CommandArgs commandArgs,
                                   const std::function<void(const std::string &text)> onMessage) {
    if (appContext_ == nullptr) {
        return false;
    }
    onMessage(appContext_->GetCommandManager()->GetHelpText(appContext_->GetLangsResources()));
    return true;
}
