//
// Created by Cold-Mint on 2025/10/15.
//

#include "HelpCommand.h"

#include <chrono>

#include "../../Constants.h"
#include "../../scene/AppContext.h"
#include "fmt/color.h"


void glimmer::HelpCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
}

std::string glimmer::HelpCommand::GetName() const {
    return HELP_COMMAND_NAME;
}

void glimmer::HelpCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
}


bool glimmer::HelpCommand::Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) {
    auto &commands = appContext_->GetCommandManager()->GetCommands();
    onMessage(fmt::format(
        fmt::runtime(appContext_->GetLangsResources()->commandInfo),
        commands.size()));
    for (const auto &[name, command]: commands) {
        onMessage(name + ": RequiresWorldContext=" + (command->RequiresWorldContext() ? "true" : "false"));
    }
    return true;
}
