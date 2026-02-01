//
// Created by coldmint on 2026/1/30.
//

#ifndef GLIMMERWORKS_LOOTTABLECOMMAND_H
#define GLIMMERWORKS_LOOTTABLECOMMAND_H
#include "core/console/Command.h"

namespace glimmer {
    class LootCommand : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:
        explicit LootCommand(AppContext *ctx)
            : Command(ctx) {
        }

        [[nodiscard]] std::string GetName() const override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;

        [[nodiscard]] bool RequiresWorldContext() const override;

        bool Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) override;
    };
}

#endif //GLIMMERWORKS_LOOTTABLECOMMAND_H
