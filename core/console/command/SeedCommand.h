//
// Created by Cold-Mint on 2026/1/6.
//

#ifndef GLIMMERWORKS_SEEDCOMMAND_H
#define GLIMMERWORKS_SEEDCOMMAND_H
#include "../Command.h"

namespace glimmer {
    class SeedCommand : public Command {
    public:
        explicit SeedCommand(AppContext *ctx)
            : Command(ctx) {
        }

        [[nodiscard]] bool RequiresWorldContext() const override;

        bool Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) override;

        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

        [[nodiscard]] std::string GetName() const override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;
    };
}

#endif //GLIMMERWORKS_SEEDCOMMAND_H
