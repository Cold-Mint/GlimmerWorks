//
// Created by Cold-Mint on 2025/10/15.
//

#ifndef GLIMMERWORKS_HELPCOMMAND_H
#define GLIMMERWORKS_HELPCOMMAND_H
#include "../Command.h"


namespace glimmer {
    class HelpCommand final : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:
        explicit HelpCommand(AppContext *ctx) : Command(ctx) {
        }

        ~HelpCommand() override = default;

        [[nodiscard]] std::string GetName() const override;


        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;

        bool Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onOutput) override;
    };
}

#endif //GLIMMERWORKS_HELPCOMMAND_H
