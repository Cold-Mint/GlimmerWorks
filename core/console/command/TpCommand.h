//
// Created by Cold-Mint on 2025/11/16.
//

#ifndef GLIMMERWORKS_TPCOMMAND_H
#define GLIMMERWORKS_TPCOMMAND_H
#include "../Command.h"

namespace glimmer {
    class TpCommand final : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:
        explicit TpCommand(AppContext *ctx);

        ~TpCommand() override = default;

        [[nodiscard]] std::string GetName() const override;

        bool Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;

        [[nodiscard]] bool RequiresWorldContext() const override;
    };
}


#endif //GLIMMERWORKS_TPCOMMAND_H
