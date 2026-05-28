//
// Created by Cold-Mint on 2026/5/13.
//

#include "CommandRequest.h"


glimmer::CommandRequest::CommandRequest(const uint32_t id, const std::string &command, CommandSender *commandSender) {
    id_ = id;
    command_ = command;
    commandSender_ = commandSender;
}

uint32_t glimmer::CommandRequest::GetId() const {
    return id_;
}

const std::string &glimmer::CommandRequest::GetCommand() {
    return command_;
}


const glimmer::CommandSender *glimmer::CommandRequest::GetCommandSender() const {
    return commandSender_;
}
