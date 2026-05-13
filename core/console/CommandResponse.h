//
// Created by coldmint on 2026/5/13.
//

#ifndef GLIMMERWORKS_COMMANDREPONSE_H
#define GLIMMERWORKS_COMMANDREPONSE_H
#include <string>

#include "CommandResult.h"

namespace glimmer {
    class CommandResponse {
        CommandResult commandResult_ = CommandResult::Failure;
        std::string command_;

    public:
        void SetCommandResult(CommandResult commandResult, const std::string &command);

        [[nodiscard]] const std::string &GetCommand() const;

        [[nodiscard]] CommandResult GetCommandResult() const;
    };
}


#endif //GLIMMERWORKS_COMMANDREPONSE_H
