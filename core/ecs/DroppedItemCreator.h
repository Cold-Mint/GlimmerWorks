//
// Created by coldmint on 2026/3/24.
//

#pragma once
#include "IPersistenceEntityCreator.h"

namespace glimmer {
    class WorldContext;

    class DroppedItemCreator : public IPersistenceEntityCreator {
    public:
        explicit DroppedItemCreator(WorldContext *worldContext);

        static EntityItemMessage GetEntityItemMessage(WorldVector2D position, std::unique_ptr<Item> item,
                                                      float pickupCooldown);

        static ResourceRef GetResourceRef();

        void LoadTemplateComponents(GameEntity::ID id, const ResourceRef &resourceRef) override;

        void MergeEntityItemMessage(GameEntity::ID id, const EntityItemMessage &entityItemMessage) override;
    };
}
