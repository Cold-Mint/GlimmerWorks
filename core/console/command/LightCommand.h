//
// Created by coldmint on 2026/5/3.
//
#pragma once
#if  !defined(NDEBUG)
#include "core/console/Command.h"

namespace glimmer {
    class LightCommand : public Command {
    public:
        explicit LightCommand(AppContext *appContext);

    protected:
        void InitSuggestions(NodeTree<std::string> *suggestionsTree) override;

    public:
        [[nodiscard]] bool RequiresWorldContext() const override;

        [[nodiscard]] std::string GetName() const override;

        void PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) override;

        bool Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                     const std::function<void(const std::string &text)> *onMessage) override;
    };
}
#endif
