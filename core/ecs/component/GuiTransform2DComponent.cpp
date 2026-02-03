//
// Created by coldmint on 2026/1/15.
//

#include "GuiTransform2DComponent.h"

#include "core/Constants.h"
#include "src/saves/gui_transform_2d.pb.h"

void glimmer::GuiTransform2DComponent::SetPosition(const CameraVector2D position) {
    position_ = position;
}

void glimmer::GuiTransform2DComponent::SetSize(const CameraVector2D size) {
    size_ = size;
}

CameraVector2D glimmer::GuiTransform2DComponent::GetPosition() const {
    return position_;
}

CameraVector2D glimmer::GuiTransform2DComponent::GetSize() const {
    return size_;
}

bool glimmer::GuiTransform2DComponent::IsSerializable() {
    return true;
}

std::string glimmer::GuiTransform2DComponent::Serialize() {
    GuiTransform2dMessage guiTransform2dMessage;
    Vector2DMessage *positionMessage = guiTransform2dMessage.mutable_position();
    positionMessage->set_x(position_.x);
    positionMessage->set_y(position_.y);
    Vector2DMessage *sizeMessage = guiTransform2dMessage.mutable_size();
    sizeMessage->set_x(size_.x);
    sizeMessage->set_y(size_.y);
    return guiTransform2dMessage.SerializeAsString();
}

void glimmer::GuiTransform2DComponent::Deserialize(WorldContext *worldContext, const std::string &data) {
    GuiTransform2dMessage guiTransform2dMessage;
    guiTransform2dMessage.ParseFromString(data);
    position_.x = guiTransform2dMessage.position().x();
    position_.y = guiTransform2dMessage.position().y();
    size_.x = guiTransform2dMessage.position().x();
    size_.y = guiTransform2dMessage.position().y();
}

u_int32_t glimmer::GuiTransform2DComponent::GetId() {
    return COMPONENT_ID_GUI_TRANSFORM_2D;
}
