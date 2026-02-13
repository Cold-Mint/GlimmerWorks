//
// Created by Cold-Mint on 2025/11/28.
//

#ifndef GLIMMERWORKS_VFSCOMMAND_H
#define GLIMMERWORKS_VFSCOMMAND_H
#include "../Command.h"
#include "../../vfs/VirtualFileSystem.h"

namespace glimmer {
    class VFSCommand final : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:
        explicit VFSCommand(AppContext *ctx);

        [[nodiscard]] std::string GetName() const override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;

        bool Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) override;

        [[nodiscard]] NodeTree<std::string> GetSuggestionsTree(const CommandArgs &commandArgs) override;
    };
}

#endif //GLIMMERWORKS_VFSCOMMAND_H
