//
// Created by Cold-Mint on 2025/11/27.
//

#ifndef GLIMMERWORKS_ASSETVIEWERCOMMAND_H
#define GLIMMERWORKS_ASSETVIEWERCOMMAND_H
#include "../Command.h"

namespace glimmer {
    class AssetViewerCommand final : public Command {
    public:
        explicit AssetViewerCommand(AppContext *ctx)
            : Command(ctx) {
        }

    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:
        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] bool RequiresWorldContext() const override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;

        bool Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) override;
    };
}

#endif //GLIMMERWORKS_ASSETVIEWERCOMMAND_H
