//
// Created by Cold-Mint on 2026/3/3.
//

#pragma once
#include "core/console/Command.h"

namespace glimmer {
    class SummonCommand : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> *suggestionsTree) override;

    public:
        explicit SummonCommand(AppContext *appContext);

        [[nodiscard]] bool RequiresWorldContext() const override;

        bool Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                     const std::function<void(const std::string &text)> *onMessage) override;

        [[nodiscard]] std::string GetName() const override;

        void PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) override;
    };
}
