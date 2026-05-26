//
// Created by coldmint on 2026/5/24.
//
#pragma once
#if  !defined(NDEBUG)
#include "core/console/Command.h"

namespace glimmer
{
    class TileSnapshotCommand : public Command
    {
    protected:
        void InitSuggestions(NodeTree<std::string>* suggestionsTree) override;

    public:
        explicit TileSnapshotCommand(AppContext* appContext);
        [[nodiscard]] std::string GetName() const override;
        [[nodiscard]] bool RequiresWorldContext() const override;
        void PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings) override;
        bool Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                     const std::function<void(const std::string& text)>* onMessage) override;
    };
}
#endif
