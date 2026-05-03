//
// Created by coldmint on 2026/3/28.
//

#ifndef GLIMMERWORKS_FLYCOMMAND_H
#define GLIMMERWORKS_FLYCOMMAND_H
#include "core/console/Command.h"


namespace glimmer {
    class FlyCommand final : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:
        explicit FlyCommand(AppContext *appContext);

        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] bool RequiresWorldContext() const override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;

        bool Execute(const CommandSender *commandSender, CommandArgs commandArgs,
            std::function<void(const std::string &text)> onMessage) override;
    };
}


#endif //GLIMMERWORKS_FLYCOMMAND_H
