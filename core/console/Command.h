//
// Created by Cold-Mint on 2025/10/15.
//

#ifndef GLIMMERWORKS_COMMAND_H
#define GLIMMERWORKS_COMMAND_H
#include <functional>
#include <string>

#include "CommandArgs.h"

namespace Glimmer {
    class Command {
    public:
        virtual ~Command() = default;

        [[nodiscard]] virtual std::string getName() const = 0;

        virtual void InitSuggest() = 0;

        virtual bool execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onOutput) = 0;
    };
}


#endif //GLIMMERWORKS_COMMAND_H
