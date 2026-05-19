//
// Created by Cold-Mint on 2025/11/28.
//
#pragma once
#if  !defined(NDEBUG)
#include "core/console/Command.h"

namespace glimmer {
    class VFSCommand final : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> *suggestionsTree) override;

    public:
        explicit VFSCommand(AppContext *appContext);

        [[nodiscard]] std::string GetName() const override;


        void PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) override;

        [[nodiscard]] NodeTree<std::string> *GetSuggestionsTree(const CommandArgs *commandArgs) override;

        bool Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                     const std::function<void(const std::string &text)> *onMessage) override;
    };
}
#endif
