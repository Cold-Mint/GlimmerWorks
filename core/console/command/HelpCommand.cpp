//
// Created by Cold-Mint on 2025/10/15.
//

#include "HelpCommand.h"

#include <chrono>

#include "../../Constants.h"
#include "../../log/LogCat.h"


std::string glimmer::HelpCommand::GetName() const {
    return HELP_COMMAND_NAME;
}

void glimmer::HelpCommand::InitSuggest() {
}

bool glimmer::HelpCommand::Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onOutput) {
    LogCat::d("HelpCommand executed");
    return false;
}
