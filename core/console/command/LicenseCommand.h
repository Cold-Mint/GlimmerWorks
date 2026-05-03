//
// Created by Cold-Mint on 2025/12/9.
//

#ifndef GLIMMERWORKS_LICENSECOMMAND_H
#define GLIMMERWORKS_LICENSECOMMAND_H
#include "../Command.h"

namespace glimmer {
    class LicenseCommand : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;
    public:
        explicit LicenseCommand(AppContext *appContext);

        bool Execute(const CommandSender *commandSender, CommandArgs commandArgs,
            std::function<void(const std::string &text)> onMessage) override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;

        [[nodiscard]] std::string GetName() const override;
    };
}

#endif //GLIMMERWORKS_LICENSECOMMAND_H
