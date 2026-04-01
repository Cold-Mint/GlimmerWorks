//
// Created by Cold-Mint on 2026/1/15.
//

#ifndef GLIMMERWORKS_ECSCOMMAND_H
#define GLIMMERWORKS_ECSCOMMAND_H
#include "core/console/Command.h"
#include "core/ecs/GameEntity.h"

namespace glimmer {
    class EcsCommand final : public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:
        explicit EcsCommand(AppContext *appContext);

        [[nodiscard]] std::string EntityToString(GameEntity::ID gameEntityId) const;

        [[nodiscard]] std::string GetName() const override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;

        bool Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) override;

        [[nodiscard]] bool RequiresWorldContext() const override;
    };
}

#endif //GLIMMERWORKS_ECSCOMMAND_H
