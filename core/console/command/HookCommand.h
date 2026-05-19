//
// Created by coldmint on 2026/5/1.
//
#pragma once
#if  !defined(NDEBUG)
#include "core/console/Command.h"

namespace glimmer {
    class HookCommand final : public Command {
    public:
        explicit HookCommand(AppContext *appContext);

    protected:
        void InitSuggestions(NodeTree<std::string> *suggestionsTree) override;

    public:
        [[nodiscard]] std::string GetName() const override;

        void PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) override;

        [[nodiscard]] NodeTree<std::string> *GetSuggestionsTree(const CommandArgs *commandArgs) override;

        bool Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                     const std::function<void(const std::string &text)> *onMessage) override;
    };
}

#endif
