//
// Created by Cold-Mint on 2026/3/28.
//

#pragma once
#include "IEntityCreator.h"
#include "IPersistenceEntityCreator.h"

namespace glimmer {
    class FlowingTextCreator : public IEntityCreator {
        std::string text_;
        WorldVector2D position_;

    public:
        explicit FlowingTextCreator(WorldContext *worldContext, const std::string &text, WorldVector2D position);

        void LoadTemplateComponents(GameEntity::ID id) override;
    };
}
