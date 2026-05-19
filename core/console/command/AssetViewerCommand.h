//
// Created by Cold-Mint on 2025/11/27.
//
#pragma once
#if  !defined(NDEBUG)
#include "core/console/Command.h"

namespace glimmer {
    class AssetViewerCommand final : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> *suggestionsTree) override;

    public:
        explicit AssetViewerCommand(AppContext *appContext);

        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] bool RequiresWorldContext() const override;

        void PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) override;

        bool Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                     const std::function<void(const std::string &text)> *onMessage) override;
    };
}
#endif
