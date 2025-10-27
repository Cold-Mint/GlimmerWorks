//
// Created by Cold-Mint on 2025/10/15.
//

#ifndef GLIMMERWORKS_HELPCOMMAND_H
#define GLIMMERWORKS_HELPCOMMAND_H
#include "../Command.h"


namespace glimmer {
    class HelpCommand final : public Command {
    public:
        ~HelpCommand() override = default;

        [[nodiscard]] std::string GetName() const override;

        void InitSuggest() override;

        bool Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onOutput) override;
    };
}

#endif //GLIMMERWORKS_HELPCOMMAND_H
