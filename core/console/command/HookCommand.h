//
// Created by coldmint on 2026/5/1.
//

#ifndef GLIMMERWORKS_HOOKCOMMAND_H
#define GLIMMERWORKS_HOOKCOMMAND_H
#include "core/console/Command.h"

namespace glimmer {
    class HookCommand  final : public Command{
    public:
        explicit HookCommand(AppContext *appContext);

    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:

        [[nodiscard]] std::string GetName() const override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;

        bool Execute(const CommandSender *commandSender, CommandArgs commandArgs,
            std::function<void(const std::string &text)> onMessage) override;

        [[nodiscard]] NodeTree<std::string> GetSuggestionsTree(const CommandArgs &commandArgs) override;
    };
}


#endif //GLIMMERWORKS_HOOKCOMMAND_H
