//
// Created by Cold-Mint on 2026/1/15.
//

#include "GuiTransform2DComponent.h"

#include "core/Constants.h"

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
    return false;
}

GameComponentTypeMessage glimmer::GuiTransform2DComponent::GetComponentType() {
    return COMPONENT_GUI_TRANSFORM_2D;
}
