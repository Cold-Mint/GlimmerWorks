//
// Created by Cold-Mint on 2025/12/23.
//

#ifndef GLIMMERWORKS_GIVECOMMAND_H
#define GLIMMERWORKS_GIVECOMMAND_H
#include "../Command.h"

namespace glimmer {
    class GiveCommand : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:
        explicit GiveCommand(AppContext *appContext);

        bool Execute(const CommandSender *commandSender, CommandArgs commandArgs,
            std::function<void(const std::string &text)> onMessage) override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;

        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] bool RequiresWorldContext() const override;
    };
}

#endif //GLIMMERWORKS_GIVECOMMAND_H
