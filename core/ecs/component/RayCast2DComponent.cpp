//
// Created by Cold-Mint on 2026/3/8.
//

#include "RayCast2DComponent.h"

const WorldVector2D &glimmer::RayCast2DComponent::GetOrigin() const {
    return origin_;
}

void glimmer::RayCast2DComponent::SetOrigin(const WorldVector2D &origin) {
    origin_ = origin;
}

const b2QueryFilter &glimmer::RayCast2DComponent::GetFilter() const {
    return filter_;
}

void glimmer::RayCast2DComponent::SetFilter(const b2QueryFilter &filter) {
    filter_ = filter;
}

void glimmer::RayCast2DComponent::SetTransform(const WorldVector2D &transform) {
    translation_ = transform;
}

const WorldVector2D &glimmer::RayCast2DComponent::GetHitPoint() const {
    return hitPoint_;
}

void glimmer::RayCast2DComponent::SetHitPoint(const WorldVector2D &hitPoint) {
    hitPoint_ = hitPoint;
}

const b2ShapeId &glimmer::RayCast2DComponent::GetHitShape() const {
    return hitShapeId_;
}

void glimmer::RayCast2DComponent::SetHitShape(const b2ShapeId &hitShapeId) {
    hitShapeId_ = hitShapeId;
}

const WorldVector2D &glimmer::RayCast2DComponent::GetHitNormal() const {
    return hitNormal_;
}

void glimmer::RayCast2DComponent::SetHitNormal(const WorldVector2D &hitNormal) {
    hitNormal_ = hitNormal;
}

bool glimmer::RayCast2DComponent::IsHit() const {
    return hit_;
}

void glimmer::RayCast2DComponent::SetHit(bool hit) {
    hit_ = hit;
}

const WorldVector2D &glimmer::RayCast2DComponent::GetTranslation() const {
    return translation_;
}

void glimmer::RayCast2DComponent::SetTransform2DEntity(GameEntity::ID id) {
    transform2DEntity_ = id;
}

glimmer::GameEntity::ID glimmer::RayCast2DComponent::GetTransform2DEntity() const {
    return transform2DEntity_;
}

void glimmer::RayCast2DComponent::SetTransform2D(const WorldVector2D &transform2D) {
    translation_ = transform2D;
}

uint32_t glimmer::RayCast2DComponent::GetId() {
    return COMPONENT_ID_RAY_CAST_2D;
}
