//
// Created by Cold-Mint on 2025/10/26.
//

#include "GameEntity.h"

glimmer::GameEntity::GameEntity(const ID id) : id_(id) {
}
glimmer::GameEntity::ID glimmer::GameEntity::GetID() const {
    return id_;
}

void glimmer::GameEntity::SetPersistable(const bool persistable) {
    persistable_ = persistable;
}

bool glimmer::GameEntity::IsPersistable() const {
    return persistable_;
}
