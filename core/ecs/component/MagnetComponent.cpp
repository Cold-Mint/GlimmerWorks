//
// Created by coldmint on 2025/12/22.
//

#include "MagnetComponent.h"

#include <algorithm>

void glimmer::MagnetComponent::SetType(const uint16_t type) {
    type_ = type;
}

uint16_t glimmer::MagnetComponent::GetType() const {
    return type_;
}

void glimmer::MagnetComponent::SetDetectionRadius(const float detectionRadius) {
    detectionRadius_ = detectionRadius;
}

float glimmer::MagnetComponent::GetDetectionRadius() const {
    return detectionRadius_;
}

void glimmer::MagnetComponent::SetAdsorptionRadius(const float adsorptionRadius) {
    adsorptionRadius_ = adsorptionRadius;
}

float glimmer::MagnetComponent::GetAdsorptionRadius() const {
    return adsorptionRadius_;
}

void glimmer::MagnetComponent::RemoveEntity(GameEntity::ID entityId) {
    entities_.erase(
        std::ranges::remove(entities_, entityId).begin(),
        entities_.end()
    );
}

void glimmer::MagnetComponent::AddEntity(GameEntity::ID entityId) {
    if (std::find(entities_.begin(), entities_.end(), entityId) == entities_.end()) {
        entities_.push_back(entityId);
    }
}

const std::vector<glimmer::GameEntity::ID> &glimmer::MagnetComponent::GetEntities() const {
    return entities_;
}

u_int32_t glimmer::MagnetComponent::GetId() {
    return COMPONENT_ID_MAGNET;
}
