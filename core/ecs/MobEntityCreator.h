//
// Created by Cold-Mint on 2026/3/24.
//

#pragma once
#include "IPersistenceEntityCreator.h"

namespace glimmer {
    class MobEntityCreator : public IPersistenceEntityCreator {
    public:
        explicit MobEntityCreator(WorldContext *worldContext);

        static EntityItemMessage GetEntityItemMessage(WorldVector2D position);

        void LoadTemplateComponents(GameEntity::ID id, const ResourceRef &resourceRef) override;

        void MergeEntityItemMessage(GameEntity::ID id, const EntityItemMessage &entityItemMessage) override;
    };
}
