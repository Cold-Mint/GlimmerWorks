//
// Created by coldmint on 2025/12/22.
//

#include "MagneticComponent.h"

#include "core/Constants.h"
#include "src/saves/magnetic.pb.h"

void glimmer::MagneticComponent::SetType(uint16_t type) {
    type_ = type;
}

bool glimmer::MagneticComponent::IsTweening() const {
    return tweening_;
}

void glimmer::MagneticComponent::SetTweening(bool tweening) {
    tweening_ = tweening;
}

tweeny::tween<float> &glimmer::MagneticComponent::GetTween() {
    return tween_;
}

void glimmer::MagneticComponent::SetTween(const tweeny::tween<float> &tween) {
    tween_ = tween;
}


const WorldVector2D &glimmer::MagneticComponent::GetStartPos() const {
    return startPos_;
}

void glimmer::MagneticComponent::SetStartPos(const WorldVector2D &startPos) {
    startPos_ = startPos;
}

uint16_t glimmer::MagneticComponent::GetType() const {
    return type_;
}

u_int32_t glimmer::MagneticComponent::GetId() {
    return COMPONENT_ID_MAGNETIC;
}

bool glimmer::MagneticComponent::IsSerializable() {
    return true;
}

std::string glimmer::MagneticComponent::Serialize() {
    MagneticMessage magneticMessage;
    magneticMessage.set_type(type_);
    return magneticMessage.SerializeAsString();
}

void glimmer::MagneticComponent::Deserialize(AppContext *appContext, WorldContext *worldContext,
                                             const std::string &data) {
    MagneticMessage magneticMessage;
    magneticMessage.ParseFromString(data);
    type_ = magneticMessage.type();
}
