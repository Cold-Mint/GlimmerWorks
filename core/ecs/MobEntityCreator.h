//
// Created by coldmint on 2026/3/24.
//

#ifndef GLIMMERWORKS_MOBENTITYCREATOR_H
#define GLIMMERWORKS_MOBENTITYCREATOR_H
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

#endif //GLIMMERWORKS_MOBENTITYCREATOR_H
