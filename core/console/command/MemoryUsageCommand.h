//
// Created by Cold-Mint on 2026/5/18.
//
#pragma once
#if  !defined(NDEBUG)
#include "core/console/Command.h"

namespace glimmer {
    class MemoryUsageCommand : public Command {
    public:
        explicit MemoryUsageCommand(AppContext *appContext);

        static std::string BytesToReadableUnit(uint64_t byteSize);

    protected:
        void InitSuggestions(NodeTree<std::string> *suggestionsTree) override;

    public:
        [[nodiscard]] std::string GetName() const override;


        void PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) override;

        bool Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                     const std::function<void(const std::string &text)> *onMessage) override;
    };
}
#endif
