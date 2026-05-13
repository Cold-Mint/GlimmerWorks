//
// Created by coldmint on 2026/5/13.
//

#include "CommandResponse.h"

void glimmer::CommandResponse::SetCommandResult(const CommandResult commandResult, const std::string &command) {
    commandResult_ = commandResult;
    command_ = command;
}

const std::string &glimmer::CommandResponse::GetCommand() const {
    return command_;
}

glimmer::CommandResult glimmer::CommandResponse::GetCommandResult() const {
    return commandResult_;
}
