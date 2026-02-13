//
// Created by coldmint on 2026/2/13.
//

#ifndef GLIMMERWORKS_STRUCTURECOMMAND_H
#define GLIMMERWORKS_STRUCTURECOMMAND_H
#include "core/console/Command.h"

namespace glimmer {
    class StructureCommand : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:
        explicit StructureCommand(AppContext *ctx);


        [[nodiscard]] std::string GetName() const override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;

        [[nodiscard]] bool RequiresWorldContext() const override;

        bool Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) override;
    };
}


#endif //GLIMMERWORKS_STRUCTURECOMMAND_H
