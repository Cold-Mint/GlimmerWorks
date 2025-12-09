//
// Created by Cold-Mint on 2025/12/9.
//

#ifndef GLIMMERWORKS_LICENSECOMMAND_H
#define GLIMMERWORKS_LICENSECOMMAND_H
#include "../Command.h"
#include "../../vfs/VirtualFileSystem.h"

namespace glimmer {
    class LicenseCommand : public Command {
        VirtualFileSystem *virtualFileSystem_;

    public:
        explicit LicenseCommand(AppContext *ctx, VirtualFileSystem *virtualFileSystem)
            : Command(ctx), virtualFileSystem_(virtualFileSystem) {
        }

        bool Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onOutput) override;

        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;
 [[nodiscard]] std::string GetName() const override;
    };
}

#endif //GLIMMERWORKS_LICENSECOMMAND_H
