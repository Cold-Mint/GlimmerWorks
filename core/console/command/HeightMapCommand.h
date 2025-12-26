//
// Created by Cold-Mint on 2025/12/14.
//

#ifndef GLIMMERWORKS_HEIGHTMAPCOMMAND_H
#define GLIMMERWORKS_HEIGHTMAPCOMMAND_H
#include "../Command.h"
#include "../../vfs/VirtualFileSystem.h"

namespace glimmer {
    class HeightMapCommand : public Command {
        VirtualFileSystem *virtualFileSystem_;

    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:
        explicit HeightMapCommand(AppContext *ctx, VirtualFileSystem *virtualFileSystem)
            : Command(ctx), virtualFileSystem_(virtualFileSystem) {
        }

        ~HeightMapCommand() override = default;

        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] bool RequiresWorldContext() const override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;

        [[nodiscard]] NodeTree<std::string> GetSuggestionsTree(const CommandArgs &commandArgs) override;

        bool Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) override;
    };
}

#endif //GLIMMERWORKS_HEIGHTMAPCOMMAND_H
