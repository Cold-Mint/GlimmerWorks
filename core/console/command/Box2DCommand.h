//
// Created by Cold-Mint on 2025/11/16.
//
#if  !defined(NDEBUG)
#ifndef GLIMMERWORKS_BOX2DCOMMAND_H
#define GLIMMERWORKS_BOX2DCOMMAND_H
#include "../Command.h"

namespace glimmer {
    class Box2DCommand final : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> *suggestionsTree) override;

    public:
        explicit Box2DCommand(AppContext *appContext);

        ~Box2DCommand() override = default;

        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] bool RequiresWorldContext() const override;

        void PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) override;

        bool Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                     const std::function<void(const std::string &text)> *onMessage) override;
    };
}

#endif //GLIMMERWORKS_BOX2DCOMMAND_H
#endif
