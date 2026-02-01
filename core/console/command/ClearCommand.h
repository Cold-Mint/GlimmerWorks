//
// Created by coldmint on 2026/1/30.
//

#ifndef GLIMMERWORKS_CLEARCOMMAND_H
#define GLIMMERWORKS_CLEARCOMMAND_H
#include "core/console/Command.h"

namespace glimmer {
    class ClearCommand final : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:
        explicit ClearCommand(AppContext *ctx)
            : Command(ctx) {
        }

        [[nodiscard]] std::string GetName() const override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;

        bool Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) override;

        [[nodiscard]] bool RequiresWorldContext() const override;
    };
}

#endif //GLIMMERWORKS_CLEARCOMMAND_H
