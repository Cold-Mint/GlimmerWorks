//
// Created by Cold-Mint on 2026/3/3.
//

#ifndef GLIMMERWORKS_SUMMONCOMMAND_H
#define GLIMMERWORKS_SUMMONCOMMAND_H
#include "core/console/Command.h"

namespace glimmer {
    class SummonCommand : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:
        explicit SummonCommand(AppContext *appContext);

        [[nodiscard]] bool RequiresWorldContext() const override;

        bool Execute(const CommandSender *commandSender, CommandArgs commandArgs,
            std::function<void(const std::string &text)> onMessage) override;

        [[nodiscard]] std::string GetName() const override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;
    };
}


#endif //GLIMMERWORKS_SUMMONCOMMAND_H
