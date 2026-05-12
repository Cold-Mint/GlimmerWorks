//
// Created by coldmint on 2026/5/12.
//

#ifndef GLIMMERWORKS_BIOMESCORECOMMAND_H
#define GLIMMERWORKS_BIOMESCORECOMMAND_H
#include "core/console/Command.h"

namespace glimmer {
    class BiomeScoreCommand final : public Command {
    public:
        explicit BiomeScoreCommand(AppContext *appContext);

    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:
        [[nodiscard]] std::string GetName() const override;

        [[nodiscard]] bool RequiresWorldContext() const override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;

        bool Execute(const CommandSender *commandSender, CommandArgs commandArgs,
            std::function<void(const std::string &text)> onMessage) override;
    };
}

#endif //GLIMMERWORKS_BIOMESCORECOMMAND_H
