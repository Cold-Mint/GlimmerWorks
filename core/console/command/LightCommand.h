//
// Created by coldmint on 2026/5/3.
//

#ifndef GLIMMERWORKS_LIGHTCOMMAND_H
#define GLIMMERWORKS_LIGHTCOMMAND_H
#include "core/console/Command.h"

namespace glimmer {
    class LightCommand : public Command{
    public:
        explicit LightCommand(AppContext *appContext);

    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:
        [[nodiscard]] bool RequiresWorldContext() const override;

        [[nodiscard]] std::string GetName() const override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;

        bool Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) override;
    };
}

#endif //GLIMMERWORKS_LIGHTCOMMAND_H
