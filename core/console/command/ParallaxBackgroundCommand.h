//
// Created by coldmint on 2026/5/10.
//

#ifndef GLIMMERWORKS_PARALLAXBACKGROUNDCOMMAND_H
#define GLIMMERWORKS_PARALLAXBACKGROUNDCOMMAND_H
#include "core/console/Command.h"

namespace glimmer {
    class ParallaxBackgroundCommand : public Command {
    public:
        explicit ParallaxBackgroundCommand(AppContext *appContext);

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

#endif //GLIMMERWORKS_PARALLAXBACKGROUNDCOMMAND_H
