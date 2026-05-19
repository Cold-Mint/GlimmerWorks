//
// Created by Cold-Mint on 2025/12/14.
//
#pragma once
#if  !defined(NDEBUG)
#include "core/console/Command.h"
#include "core/vfs/VirtualFileSystem.h"

namespace glimmer {
    class HeightMapCommand : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> *suggestionsTree) override;

    public:
        explicit HeightMapCommand(AppContext *appContext);

        ~HeightMapCommand() override = default;

        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] bool RequiresWorldContext() const override;

        void PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) override;

        bool Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                     const std::function<void(const std::string &text)> *onMessage) override;
    };
}
#endif
