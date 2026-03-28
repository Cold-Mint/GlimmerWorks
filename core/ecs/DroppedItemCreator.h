//
// Created by coldmint on 2026/3/24.
//

#ifndef GLIMMERWORKS_DROPEDITEMCREATOR_H
#define GLIMMERWORKS_DROPEDITEMCREATOR_H
#include "IPersistenceEntityCreator.h"
#include "core/world/WorldContext.h"

namespace glimmer {
    class DroppedItemCreator : public IPersistenceEntityCreator {

    public:
        explicit DroppedItemCreator(WorldContext *worldContext);

        static EntityItemMessage GetEntityItemMessage(WorldVector2D position, std::unique_ptr<Item> item,
                                                      float pickupCooldown);

        static ResourceRef GetResourceRef();

        void LoadTemplateComponents(GameEntity::ID id, const ResourceRef &resourceRef) override;

        void MergeEntityItemMessage(GameEntity::ID id,  const EntityItemMessage &entityItemMessage) override;
    };
}

#endif //GLIMMERWORKS_DROPEDITEMCREATOR_H
