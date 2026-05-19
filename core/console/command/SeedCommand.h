//
// Created by Cold-Mint on 2026/1/6.
//

#pragma once
#include "core/console/Command.h"

namespace glimmer {
    class SeedCommand : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> *suggestionsTree) override;

    public:
        explicit SeedCommand(AppContext *appContext);

        [[nodiscard]] bool RequiresWorldContext() const override;

        bool Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                     const std::function<void(const std::string &text)> *onMessage) override;

        [[nodiscard]] std::string GetName() const override;

        void PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) override;
    };
}