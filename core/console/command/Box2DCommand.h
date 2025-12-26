//
// Created by Cold-Mint on 2025/11/16.
//

#ifndef GLIMMERWORKS_BOX2DCOMMAND_H
#define GLIMMERWORKS_BOX2DCOMMAND_H
#include "../Command.h"

namespace glimmer {
    class Box2DCommand final : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:
        explicit Box2DCommand(AppContext *ctx)
            : Command(ctx) {
        }

        ~Box2DCommand() override = default;

        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] bool RequiresWorldContext() const override;

        bool Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;
    };
}

#endif //GLIMMERWORKS_BOX2DCOMMAND_H
