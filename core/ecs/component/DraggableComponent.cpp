//
// Created by coldmint on 2026/4/2.
//

#include "DraggableComponent.h"

#include "core/Constants.h"


void glimmer::DraggableComponent::SetSize(const CameraVector2D size) {
    size_ = size;
}

CameraVector2D glimmer::DraggableComponent::GetSize() const {
    return size_;
}

GameComponentTypeMessage glimmer::DraggableComponent::GetComponentType() {
    return COMPONENT_DRAGGABLE;
}
