//
// Created by Cold-Mint on 2025/11/16.
//

#pragma once
#include "core/console/Command.h"

namespace glimmer {
    class TpCommand final : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> *suggestionsTree) override;

    public:
        explicit TpCommand(AppContext *appContext);

        ~TpCommand() override = default;

        [[nodiscard]] std::string GetName() const override;

        void PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) override;

        bool Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                     const std::function<void(const std::string &text)> *onMessage) override;

        [[nodiscard]] bool RequiresWorldContext() const override;
    };
}
