//
// Created by Cold-Mint on 2026/5/12.
//
#pragma once
#if  !defined(NDEBUG)
#include "core/console/Command.h"

namespace glimmer {
    class BiomeScoreCommand final : public Command {
    public:
        explicit BiomeScoreCommand(AppContext *appContext);

    protected:
        void InitSuggestions(NodeTree<std::string> *suggestionsTree) override;

    public:
        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] bool RequiresWorldContext() const override;

        void PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) override;

        bool Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                     const std::function<void(const std::string &text)> *onMessage) override;
    };
}
#endif
