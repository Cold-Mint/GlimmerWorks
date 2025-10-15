//
// Created by Cold-Mint on 2025/10/15.
//

#ifndef GLIMMERWORKS_HELPCOMMAND_H
#define GLIMMERWORKS_HELPCOMMAND_H
#include "../Command.h"


namespace Glimmer {
    class HelpCommand final : public Command {
    public:
        ~HelpCommand() override = default;

        [[nodiscard]] std::string getName() const override;

        void InitSuggest() override;

        bool execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onOutput) override;
    };
}

#endif //GLIMMERWORKS_HELPCOMMAND_H
