//
// Created by Cold-Mint on 2025/12/23.
//

#pragma once
#include "core/console/Command.h"

namespace glimmer {
    class GiveCommand : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> *suggestionsTree) override;

    public:
        explicit GiveCommand(AppContext *appContext);

        void PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) override;

        bool Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                     const std::function<void(const std::string &text)> *onMessage) override;

        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] bool RequiresWorldContext() const override;
    };
}
