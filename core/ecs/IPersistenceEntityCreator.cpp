//
// Created by coldmint on 2026/3/23.
//

#include "IPersistenceEntityCreator.h"

void glimmer::IPersistenceEntityCreator::RecoveryAllComponent(const GameEntity::ID id, const EntityItemMessage &entityItemMessage) const {
    const int componentSize = entityItemMessage.components_size();
    for (int j = 0; j < componentSize; j++) {
        (void) worldContext_->RecoveryComponent(id, entityItemMessage.components(j));
    }
}

glimmer::IPersistenceEntityCreator::IPersistenceEntityCreator(WorldContext *worldContext) : worldContext_(worldContext) {
}
