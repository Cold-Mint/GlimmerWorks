//
// Created by Cold-Mint on 2025/10/28.
//

#include "Transform2DComponent.h"

#include "core/Constants.h"
#include "src/core/vector2di.pb.h"
#include "src/saves/transform2d.pb.h"

void glimmer::Transform2DComponent::SetPosition(WorldVector2D newPosition) {
    position_ = newPosition;
}

void glimmer::Transform2DComponent::SetRotation(const float newRotation) {
    rotation_ = newRotation;
}

float glimmer::Transform2DComponent::GetRotation() const {
    return rotation_;
}

void glimmer::Transform2DComponent::Translate(const WorldVector2D deltaPosition) {
    position_ += deltaPosition;
}

WorldVector2D glimmer::Transform2DComponent::GetPosition() const {
    return position_;
}

bool glimmer::Transform2DComponent::IsSerializable() {
    return true;
}

std::string glimmer::Transform2DComponent::Serialize() {
    Transform2dMessage transform2DMessage;
    auto *pos = transform2DMessage.mutable_position();
    pos->set_x(position_.x);
    pos->set_y(position_.y);
    transform2DMessage.set_rotation(rotation_);
    return transform2DMessage.SerializeAsString();
}

void glimmer::Transform2DComponent::Deserialize(AppContext *appContext, WorldContext *worldContext,const std::string &data) {
    Transform2dMessage transform2DMessage;
    transform2DMessage.ParseFromString(data);
    position_.x = transform2DMessage.position().x();
    position_.y = transform2DMessage.position().y();
    rotation_ = transform2DMessage.rotation();
}

u_int32_t glimmer::Transform2DComponent::GetId() {
    return COMPONENT_ID_TRANSFORM_2D;
}
