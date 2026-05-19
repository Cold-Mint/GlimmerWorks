//
// Created by coldmint on 2026/3/29.
//

#pragma once
#include "core/console/Command.h"

namespace glimmer {
    class ScreenshotCommand : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> *suggestionsTree) override;

    public:
        explicit ScreenshotCommand(AppContext *appContext);

        [[nodiscard]] std::string GetName() const override;

        void PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) override;

        bool Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                     const std::function<void(const std::string &text)> *onMessage) override;
    };
}
