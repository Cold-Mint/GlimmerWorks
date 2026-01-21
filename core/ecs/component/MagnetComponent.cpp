//
// Created by coldmint on 2025/12/22.
//

#include "MagnetComponent.h"

#include <algorithm>

#include "src/saves/magnet.pb.h"

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

bool glimmer::MagnetComponent::IsSerializable() {
    return true;
}

std::string glimmer::MagnetComponent::Serialize() {
    MagnetMessage magnetMessage;
    magnetMessage.set_type(type_);
    magnetMessage.set_detectionradius(detectionRadius_);
    magnetMessage.set_adsorptionradius(adsorptionRadius_);
    return magnetMessage.SerializeAsString();
}

void glimmer::MagnetComponent::Deserialize(AppContext *appContext, WorldContext *worldContext,
                                           const std::string &data) {
    MagnetMessage magnetMessage;
    magnetMessage.ParseFromString(data);
    type_ = magnetMessage.type();
    detectionRadius_ = magnetMessage.detectionradius();
    adsorptionRadius_ = magnetMessage.adsorptionradius();
}

u_int32_t glimmer::MagnetComponent::GetId() {
    return COMPONENT_ID_MAGNET;
}
